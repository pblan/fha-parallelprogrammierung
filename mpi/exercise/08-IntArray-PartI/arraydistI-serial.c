#include <stdio.h>
#include <stdlib.h>

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
  // data memory
  const int k = 50;
  int* vector = (int*)malloc(k * sizeof(int));
  
  // initializing and printing the array
  initArray(vector,k,4); // choose 4 to compare results with 4 cores later
  printArray(vector,k);

  const int pattern = 0;
        
  int count = countPattern(vector,k,pattern);
  printf("Number %i was found %i times in the array\n", pattern, count);

  // do not forget
  free(vector);

  return EXIT_SUCCESS;
}
