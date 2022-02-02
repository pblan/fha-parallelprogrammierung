#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

/*
 * mpicc -std=c99 pingpong.c -o pingpong
 * mpiexec -np 2 pingpong
 */

int main(int argc, char* argv[]) 
{
	int procRank,procCount;

	MPI_Init(&argc,&argv);

	MPI_Comm_size(MPI_COMM_WORLD,&procCount);
	MPI_Comm_rank(MPI_COMM_WORLD,&procRank);
		
  printf("Start[%d]/[%d] \n",procRank,procCount);

  // do a check for two processes here
  
  // this is the communication partner
  int procPartner = 1-procRank;
  
  // define some tag for messages
  enum { tagPingPong = 1 };
    
  // number of ping-pongs
  int n = 3;

  if (0==procRank) // first send then recv
  {
    int data=1;
    for (int i=0; i<n; ++i) 
    {
      printf("%d sent: Ping from proc %d\n",data,procRank);
      MPI_Send(&data,1,MPI_INT,procPartner,tagPingPong,MPI_COMM_WORLD);
      MPI_Recv(&data,1,MPI_INT,procPartner,tagPingPong,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      ++data;
    }
  }
  else if (1==procRank) // first recv then send
  {
    int data=-1;
    for (int i=0; i<n; ++i) 
    {
      MPI_Recv(&data,1,MPI_INT,procPartner,tagPingPong,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      ++data;
      printf("%d sent: Pong from proc %d\n",data,procRank);
      MPI_Send(&data,1,MPI_INT,procPartner,tagPingPong,MPI_COMM_WORLD);
    }
  }

	MPI_Finalize();
	
	return EXIT_SUCCESS;
}
