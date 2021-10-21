#include <stdio.h>
#include <stdlib.h>

/*
 * gcc -o hello hello.c
 * ./hello
 * mpiexec -np 3 ./hello
 */

int main(int argc, char* argv[]) 
{
	printf("Hello World\n");

  return EXIT_SUCCESS;
}
