#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  int procRank, procCount;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &procCount);
  MPI_Comm_rank(MPI_COMM_WORLD, &procRank);

  MPI_Group GROUP_ALL;
  MPI_Comm_group(MPI_COMM_WORLD, &GROUP_ALL);

  int ranks[20];

  for (int i = 0; i < 20; ++i) {
    ranks[i] = i;
  }

  MPI_Group GROUP_20;
  MPI_Group_incl(GROUP_ALL, 20, ranks, &GROUP_20);

  MPI_Comm COMM_20;
  MPI_Comm_create(MPI_COMM_WORLD, GROUP_20, &COMM_20);

  // split into 20 : 5 = 4 equal groups
  MPI_Comm COMM_5;
  MPI_Comm_split(COMM_20, (procRank / 5), (procRank % 5), &COMM_5);

  int procRank_new, procCount_new;

  MPI_Comm_size(COMM_5, &procCount_new);
  MPI_Comm_rank(COMM_5, &procRank_new);

  printf("Old: %2d/%2d\tNew: %d/%d\n", procRank, procCount, procRank_new,
         procCount_new);

  MPI_Finalize();

  return EXIT_SUCCESS;
}
