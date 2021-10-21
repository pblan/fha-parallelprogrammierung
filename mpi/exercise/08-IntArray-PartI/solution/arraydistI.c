#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

/*
 * mpicc -std=c99 arraydistI.c -o arraydistI
 * mpiexec -np 2 arraydistI
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

void printArray(const int v[], const int size, const int procRank) 
{
  printf("Array (proc %d):",procRank);
  for(int i=0; i<size; ++i) 
    printf(" %i", v[i]);			
 	printf("\n");
}

int main (int argc, char* argv[])
{
	int procRank,procCount;

  MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD,&procCount);
	MPI_Comm_rank(MPI_COMM_WORLD,&procRank);
  printf("Start[%d]/[%d] \n",procRank,procCount);
  
  enum { tagSend = 1 };

  if (0==procRank) // master
  { 
    // data memory
    const int k = 50;
    int* vector = (int*)malloc(k * sizeof(int));

    // initializing and printing the array
    initArray(vector,k,4); // choose 4 to compare results with 4 cores later
    printArray(vector,k,0);
    
    MPI_Send(vector, k, MPI_INT, 1, tagSend, MPI_COMM_WORLD);
    
    free(vector);
  }
  else if (1==procRank) // one worker
  {
    MPI_Status status;
    MPI_Probe(0,tagSend, MPI_COMM_WORLD, &status);

    int k;
    MPI_Get_count(&status,MPI_INT,&k);
    printf("k=%d in proc %d \n",k,procRank);
    
    int* vector = (int*)malloc(k * sizeof(int));
    
    MPI_Recv(vector, k, MPI_INT, 0, tagSend, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    
    printArray(vector,k,1);
    
    const int pattern = 0;
        
    int count = countPattern(vector,k,pattern);
    printf("Number %i was found %i times in the array\n", pattern, count);
    
    free(vector);
  }
  

  MPI_Finalize();

  return EXIT_SUCCESS;
}
