#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"

/*
 * mpicc -std=c99 heateq-serial.c -o heateq-serial
 * mpiexec -np 10 heateq-serial
 *
 * install gnuplot: sudo zypper install gnuplot
 * start gnuplot
 * set xrange [-0.1:1.1]
 * set yrange [-1.1:10.1]
 * p 'data0','dataS'
 */ 

void InitFields(double uk0[], double uk1[], int Length, 
               int n0, double d0, int n1, double d1, double dM);

void AppendFile(char* FileName, double uk[], double Offset, int Start, int Length);


const double TempLeft  = -1.0;
const double TempRight = -1.0;
const double TempMid   = 10.0;

const int N = 1000; // time steps
const int J =  100; // space discretization


int main(int argc, char* argv[]) 
{
  int procRank,procCount;

  MPI_Init(&argc,&argv);

  MPI_Comm_size(MPI_COMM_WORLD,&procCount);
  MPI_Comm_rank(MPI_COMM_WORLD,&procRank);
		
  printf("Start[%d]/[%d] \n",procRank,procCount);

  double dx = 1.0/J;
  double dt = 0.5*dx*dx;

  if (0==procRank)
  {
    remove("data0");
    remove("dataS");
    remove("dataP");

    printf("heat eq.: dx=%lf, dt=%lf\n",dx,dt);

    double* uk0 = malloc((J+1)*sizeof(double)); // take right point into account
    double* uk1 = malloc((J+1)*sizeof(double)); // we have u[0],u[1],...,u[J]
    double* ukt; // swap fields

    InitFields(uk0,uk1,J+1, 0,TempLeft, J,TempRight, TempMid);

    AppendFile("data0", uk1,0.0, 0,J+1);
    
    for (int n=1; n<=N; ++n)  // all time steps
    {
      for (int j=1; j<J; ++j) // all locations, 1..J-1
      {
        uk1[j] = uk0[j] + dt/(dx*dx) * (uk0[j-1]-2*uk0[j]+uk0[j+1]);
      }
      ukt = uk0; uk0 = uk1; uk1 = ukt;
    }

    AppendFile("dataS", uk0,0.0, 0,J+1);

    printf("end time: T=%lf\n",0.0+N*dt);
    
    free(uk1);
    free(uk0);
  }
   	
	MPI_Finalize();
	
	return EXIT_SUCCESS;
}

// initialize uk0 and uk1 with initial temps.
// - TempL at index n0
// - TempR at index n1
// - TempM elsewhere
//-------------------------------------------
void InitFields(double uk0[], double uk1[], int Length, 
               int n0, double TempL, int n1, double TempR, double TempM)
{
  for (int j=0; j<Length; ++j)
  {
    if (j==n0)
      uk0[j] = uk1[j] = TempL;   // left temp.
    else if (j==n1)
      uk0[j] = uk1[j] = TempR;   // right temp.
    else
      uk0[j] = uk1[j] = TempM;   // middle temp.
  }
}

// write uk to file
// - use Start and Length to write only these indices
// - use Offset to adjust x position
//-----------------
void AppendFile(char* FileName, double uk[], double Offset, int Start, int Length)
{
  FILE* f = fopen(FileName,"a");

  double dx = 1.0/J;
  for (int i=Start; i<Start+Length; ++i)
  {
    fprintf(f,"%lf %lf\n",Offset+dx*i,uk[i]);
  }
  
  fclose(f);
}
