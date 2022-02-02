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
  int procRank;

  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &procRank);
  enum { tag_send = 1 };

  if (procRank == 0) {
    // data memory
    const int k = 50;
    int *vector = (int *)malloc(k * sizeof(int));

    // initializing and printing the array
    initArray(vector, k, 4); // choose 4 to compare results with 4 cores later
    printArray(vector, k);

    MPI_Send(vector, k, MPI_INT, 1, tag_send, MPI_COMM_WORLD);

    // do not forget
    free(vector);
  }

  if (procRank == 1) {
    MPI_Status status;
    MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    int k;
    MPI_Get_count(&status, MPI_INT, &k);

    int *vector = (int *)malloc(k * sizeof(int));

    MPI_Recv(vector, k, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);
    const int pattern = 0;

    int count = countPattern(vector, k, pattern);
    printf("Number %i was found %i times in the array\n", pattern, count);

    // do not forget
    free(vector);
  }

  MPI_Finalize();

  return EXIT_SUCCESS;
}
