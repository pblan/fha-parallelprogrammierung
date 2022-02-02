#include <iostream>
#include <cstdlib>
#include <queue>

#include "mpi.h"

/*
 * mpicxx mergesort.cc -o mergesort
 * mpiexec -np 1 mergesort
 */

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

void merge(int* part1, int size1, int* part2, int size2, int* array)
{
  std::cout << "merge " << size1 << " " << size2 << std::endl;
  printArray(part1,size1,0);
  printArray(part2,size2,0);
  
  int i1=0, i2=0, i=0;
  while (i1<size1 && i2<size2) 
  {
    if (*(part1+i1) < *(part2+i2)) 
      *(array+(i++)) = *(part1+(i1++));
    else
      *(array+(i++)) = *(part2+(i2++));
  }
  while (i1<size1) 
    *(array+(i++)) = *(part1+(i1++));
  while (i2<size2) 
    *(array+(i++)) = *(part2+(i2++));

    printArray(array,size1+size2,0);
  std::cout << "i:" << i << std::endl;
}

void mergesort(int* array, int size)
{
  if (size<=1)
    return;
  
  int n1=size/2;
  int n2=size-n1;

  int* part1 = (int*)malloc(size*sizeof(int));
  int* part2 = part1 + n1;
  memcpy(part1,array,size*sizeof(int));
  
  mergesort(part1,n1);
  mergesort(part2,n2);
  
  merge(part1,n1,part2,n2,array);
  
}


int main(int argc, char* argv[]) 
{
  const int k=10;
	int* array = (int*)malloc(k*sizeof(int));
  
  initArray(array,k,10);
  printArray(array,k,0);
  
  mergesort(array,k);
  
  free(array);
  
	return EXIT_SUCCESS;
}
