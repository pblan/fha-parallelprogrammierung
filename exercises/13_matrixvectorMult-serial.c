#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * gcc -std=c99 matrixvectorMult-serial.c -o matrixvectorMult-serial
 * ./matrixvectorMult-serial
 */

void printMatrix(double *A, int height, int width, int info) {
  printf("proc %d \n", info);
  for (int i = 0; i < height; ++i) {
    printf("[", info);
    for (int j = 0; j < width; ++j) {
      printf("%lf ", A[i * width + j]);
    }
    printf("]\n");
  }
}

void initFields(double *A, double *x, double *b, int height, int width) {
  for (int i = 0; i < height; ++i)
    for (int j = 0; j < width; ++j)
      A[i * width + j] = (j + i) % width;
  for (int j = 0; j < width; ++j)
    x[j] = j;
  for (int i = 0; i < height; ++i)
    b[i] = 0.0;
}

int main(int argc, char *argv[]) {

  int procRank, procCount;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &procCount);
  MPI_Comm_rank(MPI_COMM_WORLD, &procRank);

  int width, height;
  double *A, *x, *b, *A_scatter, *b_scatter;

  if (procRank == 0) {
    // A*x = b
    width = 10;
    height = 4 * 4;
    A = (double *)malloc(width * height * sizeof(double));
    x = (double *)malloc(width * sizeof(double));
    b = (double *)malloc(height * sizeof(double));
    initFields(A, x, b, height, width);
    printf("matrix A");
    printMatrix(A, height, width, 0);
    printf("vector x");
    printMatrix(x, 1, width, 0);
  }

  MPI_Bcast(&width, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Bcast(&height, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  A_scatter = (double *)malloc((height / procCount) * width * sizeof(double));
  b_scatter = (double *)malloc((height / procCount) * sizeof(double));
  if (procRank != 0) {
    x = (double *)malloc(width * sizeof(double));
  }

  MPI_Bcast(x, width, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  MPI_Scatter(A, (height / procCount) * width, MPI_DOUBLE, A_scatter,
              (height / procCount) * width, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  for (int i = 0; i < (height / procCount); ++i) {
    b_scatter[i] = 0.0;
    for (int j = 0; j < width; ++j) {
      b_scatter[i] += A_scatter[i * width + j] * x[j];
    }
  }

  MPI_Gather(b_scatter, (height / procCount), MPI_DOUBLE, b,
             (height / procCount), MPI_DOUBLE, 0, MPI_COMM_WORLD);

  MPI_Finalize();

  // // original matrix vector mult.
  // for (int i = 0; i < height; ++i) {
  //   b_scatter[i] = 0.0;
  //   for (int j = 0; j < width; ++j)
  //     b_scatter[i] += A[i * width + j] * x[j];
  // }

  if (procRank == 0) {
    printf("result b ");
    printMatrix(b, 1, height, 0);
    free(A);
    free(b);
  }

  free(x);
  free(A_scatter);
  free(b_scatter);

  return EXIT_SUCCESS;
}
