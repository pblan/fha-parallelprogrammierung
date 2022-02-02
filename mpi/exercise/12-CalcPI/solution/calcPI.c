#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

/*
 * mpicc -std=c99 calcPI.c -o calcPI
 * mpiexec -np 10 calcPI
 */

double f(double x) 
{
  return 1.0 / (1.0 + x*x);
}

int main(int argc, char* argv[]) 
{
	int procRank,procCount;

	MPI_Init(&argc,&argv);

	MPI_Comm_size(MPI_COMM_WORLD,&procCount);
	MPI_Comm_rank(MPI_COMM_WORLD,&procRank);
		
  printf("Start[%d]/[%d] \n",procRank,procCount);

  int n = 10000;
  double a=0.0, b=1.0;
  double h = (b-a)/n;

  double piNum = 0.0;
  if (0==procRank) 
  {
    // serial part, just for chk.
    for (int i=0; i<n; ++i)
      piNum += f(a+(i+0.5)*h);

    piNum *= (4*h);
  }

  // parallel part
  int n_l = n/procCount;    // local n
  int i0=n_l*procRank;      // start index for proc i
  int i1=n_l*(procRank+1);  // end index for proc i

  double piNum_l = 0.0;
  for (int i=i0; i<i1; ++i)
    piNum_l += f(a+(i+0.5)*h);

  double piNum_Sum = 0.0;
  MPI_Reduce(&piNum_l,&piNum_Sum,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);

  if (0==procRank) 
  {
    piNum_Sum *= (4*h);
    printf("Pi = %10.8lf, diff to serial PI = %lg \n",piNum_Sum,piNum_Sum-piNum);
  }

  MPI_Finalize();
	
	return EXIT_SUCCESS;
}
