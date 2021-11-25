# include <iostream>
# include <cstdlib>
# include <mpi.h>

int main( int nargs, char* argv[] )
{
	// On initialise le contexte MPI qui va s'occuper :
	//    1. Créer un communicateur global, COMM_WORLD qui permet de gérer
	//       et assurer la cohésion de l'ensemble des processus créés par MPI;
	//    2. d'attribuer à chaque processus un identifiant ( entier ) unique pour
	//       le communicateur COMM_WORLD
	//    3. etc...

	MPI_Init( &nargs, &argv );
	// Pour des raison préfère toujours cloner le communicateur global
	// MPI_COMM_WORLD qui gère l'ensemble des processus lancés par MPI.
	MPI_Comm globComm;
	MPI_Comm_dup(MPI_COMM_WORLD, &globComm);
	// On interroge le communicateur global pour connaître le nombre de processus
	// qui ont été lancés par l'utilisateur :
	int nbp;
	MPI_Comm_size(globComm, &nbp);
	// On interroge le communicateur global pour connaître l'identifiant qui
	// m'a été attribué ( en tant que processus ). Cet identifiant est compris
	// entre 0 et nbp-1 ( nbp étant le nombre de processus qui ont été lancés par
	// l'utilisateur )
	int rank;
	MPI_Comm_rank(globComm, &rank);
	MPI_Status Stat;
	// On peut maintenant commencer à écrire notre programme parallèle en utilisant les
	// services offerts par MPI.
	int send[8] = {4,7,6,8,1,3,4,5};
	int increment = send[rank];
	int ajout;
	if(rank == 0 ){
		MPI_Send(&increment, 1, MPI_INT, 1,1,MPI_COMM_WORLD);

		MPI_Recv(&ajout, 1,MPI_INT, nbp-1,1,MPI_COMM_WORLD,&Stat);
		ajout++;
		std::cout << "c = " << ajout <<std::endl;


	}else{
		MPI_Recv(&ajout, 1,MPI_INT, rank-1,1,MPI_COMM_WORLD,&Stat);
		ajout++;
		std::cout << "c = " << ajout <<std::endl;

		MPI_Send(&increment, 1, MPI_INT, (rank+1)%nbp,1,MPI_COMM_WORLD);


	}
	// A la fin du programme, on doit synchroniser une dernière fois tous les processus
	// afin qu'aucun processus ne se termine pendant que d'autres processus continue à
	// tourner. Si on oublie cet instruction, on aura une plantage assuré des processus
	// qui ne seront pas encore terminés.
	MPI_Finalize();
	return EXIT_SUCCESS;
}
