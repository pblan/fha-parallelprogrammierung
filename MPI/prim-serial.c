#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "mpi.h"

/*
 * mpicc -std=c99 prim-serial.c -o prim-serial
 * mpiexec -np 1 prim-serial
 */

// started by Alexander Koch 


int isPrim(const int num) {
	//Very naive approach
	int n = sqrt((double)num);
	for(int i=2;i<n;i++)
  {
		if(num%i==0) 
      return 0;
	}
	return 1;
}

int main(int argc, char* argv[]) 
{
	int procRank,procCount;

	MPI_Init(&argc,&argv);

	MPI_Comm_size(MPI_COMM_WORLD,&procCount);
	MPI_Comm_rank(MPI_COMM_WORLD,&procRank);
		
  printf("Start[%d]/[%d] \n",procRank,procCount);


  int iPrim;
	printf("Start to search for prim from: "); fflush(stdout);
	scanf("%i",&iPrim);
	
	while (1) {
	    if (isPrim(iPrim)) 
        break;
		iPrim++;
	}

	printf("Prime found: %i\n",iPrim);

  return EXIT_SUCCESS;
}
