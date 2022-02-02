#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

/*
 * mpicc -std=c99 chain.c -o chain
 * mpiexec -np 3 chain
 */

int main(int argc, char* argv[]) 
{
	int procRank,procCount;

	MPI_Init(&argc,&argv);

	MPI_Comm_size(MPI_COMM_WORLD,&procCount);
	MPI_Comm_rank(MPI_COMM_WORLD,&procRank);
  
  printf("Start[%d]/[%d] \n",procRank,procCount);

  // send targets and recv sources
	int dest = (procRank!=procCount-1) ? procRank+1 : 0;
	int src  = (procRank!=0) ? procRank-1 : procCount-1;
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
	  if (procRank==0) 
    {
      data = 42;  
	  	MPI_Send(&data,1,MPI_INT,dest,tagSend,MPI_COMM_WORLD);
      data = -1;
		  MPI_Recv(&data,1,MPI_INT,src,tagSend,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	  } 
    else 
    {
      data = -1;
 			MPI_Recv(&data,1,MPI_INT,src,tagSend,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
  		MPI_Send(&data,1,MPI_INT,dest,tagSend,MPI_COMM_WORLD);
	  }
  }
		
  if (procRank==0) 
  {
    tm1 = MPI_Wtime();
    printf("\nFinal data %d, mean round trip time %lf \n",data,(tm1-tm0)/(double)n);
  }
	
	MPI_Finalize();
	
	return EXIT_SUCCESS;
}
