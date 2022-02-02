#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

/*
 * mpicc -std=c99 communicators.c -o communicators
 * mpiexec -np 25 communicators
 */

int main(int argc, char* argv[]) 
{
	int procRank,procCount;

	MPI_Init(&argc,&argv);

	MPI_Comm_size(MPI_COMM_WORLD,&procCount);
	MPI_Comm_rank(MPI_COMM_WORLD,&procRank);
		
  printf("Start[%d]/[%d] \n",procRank,procCount);

  int message = -1;
  enum { tagSend = 1 };
  
  // first decide whos winner
  
  int winner[20];
  for (int i=0; i<20; ++i)
    winner[i] = i;
  
  MPI_Group world_group, win_group;
  MPI_Comm_group(MPI_COMM_WORLD, &world_group);
  
  MPI_Group_incl(world_group, 20, winner, &win_group);

  // now a communicateor
  
  MPI_Comm win_comm;
  MPI_Comm_create(MPI_COMM_WORLD, win_group, &win_comm);
  
  
  if (procRank<20)
  {
    int color = procRank / 5; // that means: first 5 procs are in one comm. etc.
  
    MPI_Comm newcomm;
    MPI_Comm_split(win_comm, color, procRank, &newcomm);

    int procRankNew,procCountNew;
   
    MPI_Comm_size(newcomm, &procCountNew);
    MPI_Comm_rank(newcomm, &procRankNew);

    if (0==procRankNew) 
      message=23+color; // procs in same new communicator get same message
  
    MPI_Bcast(&message, 1, MPI_INT, 0, newcomm);
    printf("proc %d, color %d, new rank %d has Message: %d\n",procRank,color,procRankNew,message);   

    MPI_Comm_free(&newcomm);
  }
  
  if (win_comm!=MPI_COMM_NULL)
    MPI_Comm_free(&win_comm);
   	
	MPI_Finalize();
	
	return EXIT_SUCCESS;
}
