#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  int procRank, procCount;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &procCount);
  MPI_Comm_rank(MPI_COMM_WORLD, &procRank);
  enum {
    TAG_SEND = 1,
    TAG_SEND_ARRAY = 2,
    TAG_SEND_LENGTH = 3,
    TAG_SEND_PATTERN = 4
  };

  int message = 0;

  // sender
  if (procRank == 0) {
    MPI_Send(&message, 1, MPI_INT, 1, TAG_SEND, MPI_COMM_WORLD);
  }

  // receiver
  if (procRank == 1) {
    MPI_Request req;

    int flag = 0;

    MPI_Irecv(&message, 1, MPI_INT, 0, TAG_SEND, MPI_COMM_WORLD, &req);

    int count = 0;
    while (flag != 1) {
      ++count;
      MPI_Test(&req, &flag, MPI_STATUS_IGNORE);
    }

    printf("Used MPI_TEST %d times.\n", count);
  }

  MPI_Finalize();

  return EXIT_SUCCESS;
}
