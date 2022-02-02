#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

/*
 * mpicc -std=c99 broadcast-sim.c -o broadcast-sim
 * mpiexec -np 8 broadcast-sim
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
    // note: this code can be optimized, but this way it is much obvious
    // what different roles we have
    if (0==procRank) // master recv sent data
    {
 		  data = -1;
      for (int i=1; i<procCount; ++i) 
      {
        MPI_Recv(&data,1,MPI_INT,MPI_ANY_SOURCE,tagSend,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      }
    }
    else if (1==procRank) // start cascade
    {
      int dest = procRank*2;
      data = 42;
      
      MPI_Send(&data,1,MPI_INT,dest,tagSend,MPI_COMM_WORLD);
      MPI_Send(&data,1,MPI_INT,dest+1,tagSend,MPI_COMM_WORLD);
      
      MPI_Send(&data,1,MPI_INT,0,tagSend,MPI_COMM_WORLD);
    }
    else if (procRank<procCount/2) // recv data and forward
    {
      int src = procRank/2;
      int dest = procRank*2;
      data = -1;
      
      MPI_Recv(&data,1,MPI_INT,src,tagSend,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    
      MPI_Send(&data,1,MPI_INT,dest,tagSend,MPI_COMM_WORLD);
      MPI_Send(&data,1,MPI_INT,dest+1,tagSend,MPI_COMM_WORLD);
    
      MPI_Send(&data,1,MPI_INT,0,tagSend,MPI_COMM_WORLD);
    }
    else // tree leaves
    {
      int src = procRank/2;
      
      MPI_Recv(&data,1,MPI_INT,src,tagSend,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    
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
