#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  int procRank, procCount;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &procCount);
  MPI_Comm_rank(MPI_COMM_WORLD, &procRank);
  enum { SEND_ARRAY = 1, SEND_LENGTH = 2, SEND_PATTERN = 3 };

  MPI_Finalize();

  return EXIT_SUCCESS;
}
