#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

/*
 * mpicc -std=c99 matrixvectorMult.c -o matrixvectorMult
 * mpiexec -np 4 matrixvectorMult
 */

void printMatrix(double* A, int height, int width, int info) 
{
  printf("proc %d \n",info);
  for (int i=0; i<height; ++i) 
  {
    printf("[",info);
    for (int j=0; j<width; ++j) 
    {
      printf("%lf ",A[i*width+j]);
    }
    printf("]\n");
  }
}

void initFields(double* A, double* x, double* b, int height, int width)
{
  for (int i=0; i<height; ++i)
    for (int j=0; j<width; ++j)
      A[i*width+j] = (j+i)%width;
  for (int j=0; j<width; ++j)
    x[j] = j;
  for (int i=0; i<height; ++i)
    b[i] = 0.0;
}

int main (int argc, char* argv[])
{
	int procRank,procCount;

  MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD,&procCount);
	MPI_Comm_rank(MPI_COMM_WORLD,&procRank);

  printf("Start[%d]/[%d] \n",procRank,procCount);
  
  int width = 0;
  int height = 0;

  double* A = NULL;
  double* x = NULL;
  double* b = NULL;

  
  if (0==procRank) 
  {
    width = 10;
    height = 2*procCount;

    printf("width %d  height %d \n",width,height);

    A = (double*)malloc(width*height*sizeof(double));
    x = (double*)malloc(width*sizeof(double));
    b = (double*)malloc(height*sizeof(double));
  
    initFields(A,x,b,height,width);  
    printf("matrix A");
    printMatrix(A,height,width,0);
    printf("vector x");
    printMatrix(x,1,width,0);
  }

  // distribute height and width info, then x
  
  MPI_Bcast(&width,1,MPI_INT,0,MPI_COMM_WORLD);
  MPI_Bcast(&height,1,MPI_INT,0,MPI_COMM_WORLD);

  if (0!=procRank)
    x = (double*)malloc(width*sizeof(double));

  MPI_Bcast(x,width,MPI_DOUBLE,0,MPI_COMM_WORLD);

  // calculate parts of Ax=b
  
  int linesPerProc = height/procCount;
  int blockLength = linesPerProc*width;
  double* blockA = (double*)malloc(blockLength*sizeof(double));
  double* blockb = (double*)malloc(linesPerProc*sizeof(double));
  
  MPI_Scatter(A,blockLength,MPI_DOUBLE,blockA,blockLength,MPI_DOUBLE,0,MPI_COMM_WORLD);
  
  for (int i=0; i<linesPerProc; ++i) 
  {
    blockb[i]=0.0;
    for (int j=0; j<width; ++j)
      blockb[i] += blockA[i*width+j]*x[j];
  }
  
  printMatrix(blockb,1,linesPerProc,procRank);

  MPI_Gather(blockb,linesPerProc,MPI_DOUBLE,b,linesPerProc,MPI_DOUBLE,0,MPI_COMM_WORLD);

  if (0==procRank) 
  {
    printf("result b ");
    printMatrix(b,1,height,0);
  }

  MPI_Finalize();

  free(A);
  free(x);
  free(b);
  free(blockA);
  free(blockb);

  return EXIT_SUCCESS;
}
