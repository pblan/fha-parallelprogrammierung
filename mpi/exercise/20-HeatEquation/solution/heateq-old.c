#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"

/*
 * mpicc -std=c99 heateq.c -o heateq
 * mpiexec -np 10 heateq
 *
 * sudo zypper install gnuplot
 */ 


const double TempLeft  = -1.0;
const double TempRight = -1.0;
const double TempMid   = 10.0;

const double a = 0.0;
const double b = 1.0;

const int N = 10; // time steps
const int J = 10;  // space steps

void InitFields(double uk0[], double uk1[], int Length, 
               int n0, double d0, int n1, double d1, double dM)
{
  for (int j=0; j<Length; ++j)
  {
    if (j==n0)
      uk0[j] = uk1[j] = d0;
    else if (j==n1)
      uk0[j] = uk1[j] = d1;
    else
      uk0[j] = uk1[j] = dM;
  }
}

void WriteFile(char* FileName, double uk[], double Offset, int Start, int Length)
{
  FILE* f = fopen(FileName,"a");

  double dx = 1.0/J;
  for (int i=Start; i<Start+Length; ++i)
  {
    fprintf(f,"%lf %lf\n",Offset+dx*i,uk[i]);
  }
  
  fclose(f);
}

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

    double* uk0 = malloc((J+1)*sizeof(double));
    double* uk1 = malloc((J+1)*sizeof(double));

    InitFields(uk0,uk1,J+1, 0,TempLeft, J,TempRight, TempMid);

    WriteFile("data0", uk1,0.0, 0,J+1);
    
    for (int n=1; n<N; ++n)
    {
      for (int j=1; j<J; ++j)
      {
        uk1[j] = uk0[j] + dt/(dx*dx) * (uk0[j-1]-2*uk0[j]+uk0[j+1]);
      }
      double* temp = uk0; uk0 = uk1; uk1 = temp;
    }

    WriteFile("dataS", uk1,0.0, 0,J+1);
    
    free(uk1);
    free(uk0);
  }
return 0;
/*
  MPI_Barrier(MPI_COMM_WORLD);
  
  int rankLeft  = procRank-1;
  int rankRight = procRank+1;
  
  int nLocal  = NX/procCount;
  int nLocal2 = nLocal+2;
  
  double* uk0 = malloc(nLocal2*sizeof(double));
  double* uk1 = malloc(nLocal2*sizeof(double));
   
  for (int i=0; i<nLocal2; ++i)
    uk0[i] = uk1[i] = TempMid;

  if (0==procRank)
    uk0[1+0]    = uk1[1+0]    = TempLeft;
  if (procCount-1==procRank)
    uk0[1+nLocal-1] = uk1[1+nLocal-1] = TempRight;

  for (int k=0; k<NSTEPS; ++k)
  {
    if (procRank!=0)
      MPI_Send(&uk0[1],1,MPI_DOUBLE,rankLeft,0,MPI_COMM_WORLD);
    if (procRank!=procCount-1)
      MPI_Send(&uk0[nLocal],1,MPI_DOUBLE,rankRight,0,MPI_COMM_WORLD);

    if (procRank!=0)
      MPI_Recv(&uk0[0],1,MPI_DOUBLE,rankLeft,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    if (procRank!=procCount-1)
      MPI_Recv(&uk0[nLocal+1],1,MPI_DOUBLE,rankRight,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    
    for (int i=2; i<1+nLocal-1; ++i)
    {
      uk1[i] = uk0[i] + dt/(dx*dx) * (uk0[i-1]-2*uk0[i]+uk0[i+1]);
    }

    if (procRank!=0)
    {
      int i=1;
      uk1[i] = uk0[i] + dt/(dx*dx) * (uk0[i-1]-2*uk0[i]+uk0[i+1]);
    }
    if (procRank!=procCount-1)
    {
      int i=nLocal;
      uk1[i] = uk0[i] + dt/(dx*dx) * (uk0[i-1]-2*uk0[i]+uk0[i+1]);
    }
    
    double* temp = uk0; uk0 = uk1; uk1 = temp;
  }

  WriteFile("dataP", uk1,procRank*nLocal*dx-dx,1,nLocal);
    
  free(uk1);
  free(uk0);
 */
   	
	MPI_Finalize();
	
	return EXIT_SUCCESS;
}
