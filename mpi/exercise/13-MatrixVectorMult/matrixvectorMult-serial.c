#include <stdio.h>
#include <stdlib.h>

/*
 * gcc -std=c99 matrixvectorMult-serial.c -o matrixvectorMult-serial
 * ./matrixvectorMult-serial
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
  
  int width = 10;
  int height = 4*4;

  // A*x = b
  
  double* A = (double*)malloc(width*height*sizeof(double));
  double* x = (double*)malloc(width*sizeof(double));
  double* b = (double*)malloc(height*sizeof(double));
  
  initFields(A,x,b,height,width);  
  printf("matrix A");
  printMatrix(A,height,width,0);
  printf("vector x");
  printMatrix(x,1,width,0);

  // original matrix vector mult.
  for (int i=0; i<height; ++i) 
  {
    b[i]=0.0;
    for (int j=0; j<width; ++j) 
      b[i] += A[i*width+j]*x[j];
  }

  printf("result b ");
  printMatrix(b,1,height,0);

  free(A);
  free(x);
  free(b);

  return EXIT_SUCCESS;
}
