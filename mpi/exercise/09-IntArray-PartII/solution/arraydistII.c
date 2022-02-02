#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

/*
 * mpicc -std=c99 arraydistII.c -o arraydistII
 * mpiexec -np 4 arraydistII
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

  enum { tagSendVector = 1,
         tagSendPattern,
         tagSendResults };

  if(0==procRank) // MASTER PART
  {
    // data memory
    const int k = 50;
    int* vector = (int*)malloc(k * sizeof(int));
    
    // initializing and printing the array, master does his part of work
    initArray(vector,k,procCount);
    printArray(vector,k,0);
        
    // sending the array to workers
    for(int i=1; i<procCount; i++) 
    {
      MPI_Send(vector, k, MPI_INT, i, tagSendVector, MPI_COMM_WORLD);
      MPI_Send(&i, 1, MPI_INT, i, tagSendPattern, MPI_COMM_WORLD);
    }

    // get results (back) and print the results
    for(int i=0; i<procCount; i++) 
    {
        int count = -1, procID = -1;
        if (0==i)
        {
          count = countPattern(vector,k,procRank);
          procID = 0;
        }
        else 
        {
          MPI_Status status;
          MPI_Recv(&count, 1, MPI_INT, MPI_ANY_SOURCE, tagSendResults, MPI_COMM_WORLD, &status);
          procID = status.MPI_SOURCE;
        }
        
        printf("Number %i was found from proc %d %i times in the array\n", procID, procID, count);
    }

    free(vector);
  } 
  else // WORKER PART
  { 
    // wait for message
    MPI_Status status;
    MPI_Probe(0,tagSendVector, MPI_COMM_WORLD, &status);

    int k;
    MPI_Get_count(&status,MPI_INT,&k);
    printf("k=%d in proc %d \n",k,procRank);

    // data memory
    int* vector = (int*)malloc(k * sizeof(int));
    MPI_Recv(vector, k, MPI_INT, 0, tagSendVector, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    int pattern=-1;
    MPI_Recv(&pattern, 1, MPI_INT, 0, tagSendPattern, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
    int count = countPattern(vector,k,pattern);
        
    // sending results
    MPI_Send(&count, 1, MPI_INT, 0, tagSendResults, MPI_COMM_WORLD);

    free(vector);
  }

  MPI_Finalize();

  return EXIT_SUCCESS;
}
