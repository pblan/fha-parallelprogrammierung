#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

/*
 * mpicc -std=c99 arraydistI.c -o arraydistI
 * mpiexec -np 4 arraydistI
 */

int countPattern(const int v[], const int size, const int pattern){
  int count=0;

  for(int i=0; i<size; ++i)
    if(pattern==v[i])
      ++count;

  return count;
}

void initArray(int v[], const int size, const int mod) {
  for(int i=0; i<size; ++i) 
    v[i] = rand() % mod;
}

void printArray(const int v[], const int size) {
  printf("Array:\n");
  for(int i=0; i<size; ++i) 
    printf(" %i", v[i]);			
 	printf("\n\n");
}

int main (int argc, char* argv[])
{
	int procRank,procCount;

  MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD,&procCount);
	MPI_Comm_rank(MPI_COMM_WORLD,&procRank);
  printf("Start[%d]/[%d] \n",procRank,procCount);

  // data memory
  const int k = 50;
  int* vector = (int*)malloc(k * sizeof(int));
  
  // magic numbers for tags
  enum { 
    tagData = 777, 
    tagResults = 666
  };

  if(0==procRank) { // MASTER PART
    // initializing and printing the array, master does his part of work
    initArray(vector,k,procCount);
    printArray(vector,k);
        
    // sending the array to workers
    for(int i=1; i<procCount; i++) 
      MPI_Send(vector, k, MPI_INT, i, tagData, MPI_COMM_WORLD);

    // get results (back) and print the results
    for(int i=0; i<procCount; i++) {
        int count = -1;
        if (0==i)
          count = countPattern(vector,k,procRank);
        else
          MPI_Recv(&count, 1, MPI_INT, i, tagResults, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Number %i was found %i times in the array\n", i, count);
    }
  } else { // WORKER PART
    // receiving the array
    MPI_Recv(vector, k, MPI_INT, 0, tagData, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
    int count = countPattern(vector,k,procRank);
        
    // sending results
    MPI_Send(&count, 1, MPI_INT, 0, tagResults, MPI_COMM_WORLD);
  }

  free(vector);

  MPI_Finalize();

  return EXIT_SUCCESS;
}
