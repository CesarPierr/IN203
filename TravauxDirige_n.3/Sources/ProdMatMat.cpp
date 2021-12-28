#include <algorithm>
#include <cassert>
#include <iostream>
#include <thread>
#include <mutex>
#if defined(_OPENMP)
#include <omp.h>
#include <chrono>
#endif
#include "ProdMatMat.hpp"

namespace {

std::mutex operation_mutex;
void prodSubBlocks(int iRowBlkA, int iColBlkB, int iColBlkA, int szBlock,
                   const Matrix& A, const Matrix& B, Matrix& C) {
  for (int i = iRowBlkA; i < std::min(A.nbRows, iRowBlkA + szBlock); ++i)
    for (int j = iColBlkB; j < std::min(B.nbCols, iColBlkB + szBlock); j++)
      for (int k = iColBlkA; k < std::min(A.nbCols, iColBlkA + szBlock); k++)
        C(i, j) += A(i, k) * B(k, j);
}
const int szBlock = 32;
}  // namespace

Matrix operator*(const Matrix& A, const Matrix& B) {
  std::chrono::time_point<std::chrono::system_clock> start, end;
  start = std::chrono::system_clock::now();
  Matrix C(A.nbRows, B.nbCols, 0.0);
  
  prodSubBlocks(0, 0, 0, std::max({A.nbRows, B.nbCols, A.nbCols}), A, B, C);
  end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-start;
  std::cout << "calcul de la matrice " << elapsed_seconds.count() 
              << std::endl;
  return C;
}
