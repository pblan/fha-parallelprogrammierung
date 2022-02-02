#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

/*
 * mpicc -std=c99 arraydistIV.c -o arraydistIV
 * mpiexec -np 4 arraydistIV
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

void printArray(const int v[], const int size, const int id) {
  printf("Array of proc %d:\n",id);
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

  // magic numbers for tags
  enum { 
    tagResults = 666
  };

  int searchNumber = -1;
  int bufLength = -1;
  int* vector = NULL;
    
  if(0==procRank) { // MASTER PART
    // data memory
    const int k = procCount*12;
    vector = (int*)malloc(k * sizeof(int));
    
    // initializing and printing the array
    initArray(vector,k,procCount);
    printArray(vector,k,-1);
    printf("proc %d, original-len %d \n",procRank,k);
    
    bufLength = k / procCount; // works by construction
    searchNumber = rand() % procCount;
  } 
  
  // communication part, it is the same for all procs
  
  // send local buf length
  MPI_Bcast(&bufLength,1,MPI_INT,0,MPI_COMM_WORLD);

  int* searchVector = (int*)malloc(bufLength * sizeof(int));

  // send search pattern
  MPI_Bcast(&searchNumber,1,MPI_INT,0,MPI_COMM_WORLD);

  printf("proc %d, search pattern %d, local buf len %d \n",procRank,searchNumber,bufLength);

  // scatter data to local buffers
  MPI_Scatter(vector,bufLength,MPI_INT,searchVector,bufLength,MPI_INT,0,MPI_COMM_WORLD);

  printArray(searchVector,bufLength,procRank);

  int count = countPattern(searchVector,bufLength,searchNumber);
  printf("proc %d Number %i was found %i times in the array\n",procRank,searchNumber, count);
  // we do not need this anymore
  free(vector);       // allowed for NULL
  free(searchVector); // allowed for NULL
  
  int countAll = 0;
  MPI_Reduce(&count,&countAll,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
  if(0==procRank)
    printf("Number %i was found %i times overall\n", searchNumber, countAll);

  MPI_Finalize();

  return EXIT_SUCCESS;
}
