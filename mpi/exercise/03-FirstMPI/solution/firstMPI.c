#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

/*
 * mpicc firstMPI.c -o firstMPI
 * ./firstMPI
 * mpiexec -np 3 firstMPI
 */

int main(int argc, char* argv[]) 
{
	int procRank,procCount;

	MPI_Init(&argc,&argv);

	MPI_Comm_size(MPI_COMM_WORLD,&procCount);
	MPI_Comm_rank(MPI_COMM_WORLD,&procRank);
		
  printf("Start[%d]/[%d] \n",procRank,procCount);
	
	MPI_Finalize();
	
	return EXIT_SUCCESS;
}
