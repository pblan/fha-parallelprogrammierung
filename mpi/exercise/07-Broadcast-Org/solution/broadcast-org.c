#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

/*
 * mpicc -std=c99 broadcast-org.c -o broadcast-org
 * mpiexec -np 8 broadcast-org
 */

int main(int argc, char* argv[]) 
{
	int procRank,procCount;

	MPI_Init(&argc,&argv);

	MPI_Comm_size(MPI_COMM_WORLD,&procCount);
	MPI_Comm_rank(MPI_COMM_WORLD,&procRank);
  
  printf("Start[%d]/[%d] \n",procRank,procCount);
	
  // strategy: first(1)->2,3; 2->4,5;3->6,7
  // i.e. id->2*id,2*id+1 
  // master receives all results

  enum { tagSend = 1 };
  
  // we like to measure time 
  double tm0,tm1;
  
  // do loop n times    
  int n = 100;

  int data = -1;

  if (procRank==0) 
  {
    tm0 = MPI_Wtime();
  }

  for (int i=0; i<n; ++i) 
  { 
    if (0==procRank) // master recv sent data
    {
      data = 42;
      MPI_Bcast(&data,1,MPI_INT,0,MPI_COMM_WORLD);
 		  data = -1;
      for (int i=1; i<procCount; ++i) 
      {
        MPI_Recv(&data,1,MPI_INT,MPI_ANY_SOURCE,tagSend,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      }
    }
    else 
    {
      MPI_Bcast(&data,1,MPI_INT,0,MPI_COMM_WORLD);
      MPI_Send(&data,1,MPI_INT,0,tagSend,MPI_COMM_WORLD);
    }
  }
  
  if (procRank==0) 
  {
    tm1 = MPI_Wtime();
    printf("\nFinal data=%d, mean round trip time %lf \n",data,(tm1-tm0)/(double)n);
  }

 	MPI_Finalize();
	
	return EXIT_SUCCESS;
}
