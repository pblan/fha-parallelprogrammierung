#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * gcc -std=c99 arraydistI-serial.c -o arraydistI-serial
 * ./arraydistI-serial
 */

int countPattern(const int v[], const int size, const int pattern) {
  int count = 0;

  for (int i = 0; i < size; ++i) {
    if (pattern == v[i]) {
      ++count;
    }
  }

  return count;
}

void initArray(int v[], const int size, const int mod) {
  for (int i = 0; i < size; ++i) {
    v[i] = rand() % mod;
  }
}

void printArray(const int v[], const int size) {
  printf("Array:");
  for (int i = 0; i < size; ++i) {
    printf(" %i", v[i]);
  }
  printf("\n");
}

int main(int argc, char *argv[]) {
  int procRank, procCount;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &procCount);
  MPI_Comm_rank(MPI_COMM_WORLD, &procRank);
  enum { SEND_ARRAY = 1, SEND_LENGTH = 2, SEND_PATTERN = 3 };

  int *vector;
  int k;
  if (procRank == 0) {
    // data memory
    k = 50;
    vector = (int *)malloc(k * sizeof(int));

    // initializing and printing the array
    initArray(vector, k,
              (procCount - 1)); // choose procCount - 1 to compare results with
                                // procCount - 1 cores later
    printArray(vector, k);

    for (int i = 0; i < procCount; ++i) {
      int temp = k / procCount;
      MPI_Send(&temp, 1, MPI_INT, i, SEND_LENGTH, MPI_COMM_WORLD);
    }
    for (int i = 0; i < procCount; ++i) {
      int pattern = 0;
      MPI_Send(&pattern, 1, MPI_INT, i, SEND_PATTERN, MPI_COMM_WORLD);
    }
  }

  int pattern;
  MPI_Recv(&pattern, 1, MPI_INT, 0, SEND_PATTERN, MPI_COMM_WORLD,
           MPI_STATUS_IGNORE);

  int rec_count;
  MPI_Recv(&rec_count, 1, MPI_INT, 0, SEND_LENGTH, MPI_COMM_WORLD,
           MPI_STATUS_IGNORE);

  int *rec_vector = (int *)malloc(rec_count * sizeof(int));
  MPI_Scatter(vector, rec_count, MPI_INT, rec_vector, rec_count, MPI_INT, 0,
              MPI_COMM_WORLD);

  int count = countPattern(rec_vector, rec_count, pattern);

  printf("Number %i was found %i times in the array\n", pattern, count);

  int sum;
  MPI_Reduce(&count, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  if (procRank == 0) {
    printf("Final sum: %d\n", sum);
    // do not forget
    free(vector);
  }

  MPI_Finalize();

  return EXIT_SUCCESS;
}
