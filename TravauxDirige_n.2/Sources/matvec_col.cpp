// Produit matrice-vecteur
# include <cassert>
# include <vector>
# include <iostream>
# include <mpi.h>
# include <chrono>
// ---------------------------------------------------------------------
class Matrix : public std::vector<double>
{
public:
    Matrix (int dim);
    Matrix( int nrows, int ncols );
    Matrix( const Matrix& A ) = delete;
    Matrix( Matrix&& A ) = default;
    ~Matrix() = default;

    Matrix& operator = ( const Matrix& A ) = delete;
    Matrix& operator = ( Matrix&& A ) = default;
    
    double& operator () ( int i, int j ) {
        return m_arr_coefs[i + j*m_nrows];
    }
    double  operator () ( int i, int j ) const {
        return m_arr_coefs[i + j*m_nrows];
    }
    
    std::vector<double> operator * ( const std::vector<double>& u ) const;
    
    std::ostream& print( std::ostream& out ) const
    {
        const Matrix& A = *this;
        out << "[\n";
        for ( int i = 0; i < m_nrows; ++i ) {
            out << " [ ";
            for ( int j = 0; j < m_ncols; ++j ) {
                out << A(i,j) << " ";
            }
            out << " ]\n";
        }
        out << "]";
        return out;
    }
private:
    int m_nrows, m_ncols;
    std::vector<double> m_arr_coefs;
};
// ---------------------------------------------------------------------
inline std::ostream& 
operator << ( std::ostream& out, const Matrix& A )
{
    return A.print(out);
}
// ---------------------------------------------------------------------
inline std::ostream&
operator << ( std::ostream& out, const std::vector<double>& u )
{
    out << "[ ";
    for ( const auto& x : u )
        out << x << " ";
    out << " ]";
    return out;
}
// ---------------------------------------------------------------------
std::vector<double> 
Matrix::operator * ( const std::vector<double>& u ) const
{
    
    const Matrix& A = *this;
    assert( u.size() == unsigned(m_ncols) );
    std::vector<double> v(m_nrows, 0.);
    for ( int i = 0; i < m_nrows; ++i ) {
        for ( int j = 0; j < m_ncols; ++j ) {
            v[i] += A(i,j)*u[j];
        }            
    }
    return v;
}

// =====================================================================
Matrix::Matrix (int dim) : m_nrows(dim), m_ncols(dim),
                           m_arr_coefs(dim*dim)
{
    for ( int i = 0; i < dim; ++ i ) {
        for ( int j = 0; j < dim; ++j ) {
            (*this)(i,j) = (i+j)%dim;
        }
    }
}
// ---------------------------------------------------------------------
Matrix::Matrix( int nrows, int ncols ) : m_nrows(nrows), m_ncols(ncols),
                                         m_arr_coefs(nrows*ncols)
{
    int dim = (nrows > ncols ? nrows : ncols );
    for ( int i = 0; i < nrows; ++ i ) {
        for ( int j = 0; j < ncols; ++j ) {
            (*this)(i,j) = (i+j)%dim;
        }
    }    
}
// =====================================================================
int main( int nargs, char* argv[] )
{
    MPI_Init( &nargs, &argv );
	MPI_Comm globComm;
	MPI_Comm_dup(MPI_COMM_WORLD, &globComm);
	int nbp;
	MPI_Comm_size(globComm, &nbp);
	int rank;
	MPI_Comm_rank(globComm, &rank);
    const int N = 8000;
    Matrix A(N);
    std::vector<double> u( N );
    int nb_col = N/nbp;
    for ( int i = 0; i < N; ++i ) u[i] = i+1;
    if (rank == 0){
        std::chrono::time_point<std::chrono::system_clock> start, end;
        start = std::chrono::system_clock::now();
        Matrix S(N,nb_col);
        std::vector<double> u_rank(nb_col);
        for(int j = 0;j<nb_col;j++){
            u_rank[j] = u[j+rank*nb_col];
            for(int i =0; i<N; i++){
                S(i,j) = A(i,rank*nb_col+j);
            }
        }
        std::vector<double> v_rank = S*u_rank;
        //std::cout  << "A : " << A << std::endl;
        //std::cout << " u : " << u << std::endl;

        std::vector<double> buffer(N);
        MPI_Reduce (v_rank.data(), buffer.data(), N, MPI_DOUBLE, MPI_SUM, 0, globComm);
        //std::cout << "A.u = " << buffer << std::endl;
        end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end-start;
        std::cout << "Temps calcul ensemble mandelbrot : " << elapsed_seconds.count() 
                << std::endl;
    }else{
        Matrix S(N,nb_col);
        std::vector<double> u_rank(nb_col);
        for(int j = 0;j<nb_col;j++){
            u_rank[j] = u[j+rank*nb_col];
            for(int i =0; i< N; i++){
                S(i,j) = A(i,rank*nb_col+j);
            }
        }
        std::vector<double> v_rank = S*u_rank;
        std::vector<double> buffer;
        MPI_Reduce (v_rank.data(), buffer.data(), N, MPI_DOUBLE, MPI_SUM, 0, globComm);
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}
