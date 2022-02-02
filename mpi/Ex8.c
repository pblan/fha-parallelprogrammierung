#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

/*
 * gcc -std=c99 arraydistI-serial.c -o arraydistI-serial
 * ./arraydistI-serial
 */

int countPattern(const int v[], const int size, const int pattern)
{
  int count=0;

  for(int i=0; i<size; ++i)
    if(pattern==v[i])
      ++count;

  return count;
}

void initArray(int v[], const int size, const int mod) 
{
  for(int i=0; i<size; ++i) 
    v[i] = rand() % mod;
}

void printArray(const int v[], const int size) 
{
  printf("Array:");
  for(int i=0; i<size; ++i) 
    printf(" %i", v[i]);			
 	printf("\n");
}

int main (int argc, char* argv[])
{
  int procRank, procCount;
  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &procCount);
  MPI_Comm_rank(MPI_COMM_WORLD, &procRank);  

  int size;
  int pattern;
  int* vector;
  int sum;

  if(procRank == 0)
  {
    // data memory
    const int k = 50;
    size = k / procCount;
    pattern = 0;
    vector = (int*)malloc(k * sizeof(int));
    
    // initializing and printing the array
    initArray(vector,k,4); // choose 4 to compare results with 4 cores later
    printArray(vector,k);
  }

  MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);

  int* part = (int*)malloc(size * sizeof(int));

  MPI_Bcast(&pattern, 1, MPI_INT, 0, MPI_COMM_WORLD);

  MPI_Scatter(vector, size, MPI_INT, part, size, MPI_INT, 0, MPI_COMM_WORLD);
        
  int count = countPattern(part,size,pattern);

  MPI_Reduce(&count, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  if(procRank == 0)
  {
     printf("Number %i was found %i times in the array\n", pattern, sum);
    free(vector);
  }

  MPI_Finalize();

  return EXIT_SUCCESS;
}
