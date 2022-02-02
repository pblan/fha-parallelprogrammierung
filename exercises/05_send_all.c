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

  int receiver_rank = (procRank + 1) % procCount;

  if (procRank == 0) {
    MPI_Send(&message, 1, MPI_INT, receiver_rank, tag_send, MPI_COMM_WORLD);
  }

  double tm0, tm1, tm_sum;

  for (int i = 0; i < 100; ++i) {
    tm0 = MPI_Wtime();

    MPI_Recv(&message, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);
    printf("Message: %d (Process %d)\n", message++, procRank);
    MPI_Send(&message, 1, MPI_INT, receiver_rank, tag_send, MPI_COMM_WORLD);

    tm1 = MPI_Wtime();
    tm_sum += tm1 - tm0;
  }

  if (procRank == 0) {
    printf("Average time taken: %fs\n", (tm_sum / 100));
  }

  MPI_Finalize();

  return EXIT_SUCCESS;
}