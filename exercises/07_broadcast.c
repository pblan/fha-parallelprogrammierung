#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  int procRank, procCount;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &procCount);
  MPI_Comm_rank(MPI_COMM_WORLD, &procRank);

  printf("Start[%d]/[%d] \n", procRank, procCount);

  int message = 1;
  int sum = 0;
  enum { tag_send = 1 };

  double tm0, tm1, tm_sum;

  tm0 = MPI_Wtime();

  MPI_Bcast(&message, 1, MPI_INT, 0, MPI_COMM_WORLD);

  MPI_Reduce(&message, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  tm1 = MPI_Wtime();
  tm_sum += tm1 - tm0;

  if (procRank == 0) {
    printf("Average time taken: %fs\n", (tm_sum / 100));
    printf("Message (MPI_SUM): %d\n", sum);
  }

  MPI_Finalize();

  return EXIT_SUCCESS;
}