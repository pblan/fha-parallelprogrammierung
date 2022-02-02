#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  int procRank, procCount;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &procCount);
  MPI_Comm_rank(MPI_COMM_WORLD, &procRank);

  printf("Start[%d]/[%d] \n", procRank, procCount);

  int message = 0;
  enum { tag_send = 1 };
  while (1) {
    // ping
    if (procRank == 0) {
      printf("Ping! (%d)\n", message++);
      MPI_Send(&message, 1, MPI_INT, 1, tag_send, MPI_COMM_WORLD);
      MPI_Recv(&message, 1, MPI_INT, 1, MPI_ANY_TAG, MPI_COMM_WORLD,
               MPI_STATUS_IGNORE);
    }
    // pong
    if (procRank == 1) {
      MPI_Recv(&message, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD,
               MPI_STATUS_IGNORE);
      printf("Pong! (%d)\n", message++);
      MPI_Send(&message, 1, MPI_INT, 0, tag_send, MPI_COMM_WORLD);
    }
  }

  MPI_Finalize();

  return EXIT_SUCCESS;
}