#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"

/*
 * mpicc -std=c99 heateq.c -o heateq
 * mpiexec -np 10 heateq
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

const int N = 100; // time steps
const int J = 100; // space discretization


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
  
  MPI_Barrier(MPI_COMM_WORLD);
  
  // parallel but blocking
  
  {
    int rankLeft  = procRank-1;
    int rankRight = procRank+1;
    
    int JLoc = J/procCount;
    
    if (procRank==procCount-1) // take into account "last" additional point
      ++JLoc;

    double* uk0 = malloc((JLoc+2)*sizeof(double));
    double* uk1 = malloc((JLoc+2)*sizeof(double));

    for (int i=0; i<JLoc+2; ++i)
      uk0[i] = uk1[i] = TempMid;

    // we usually have an offset of 1 due to the left ghost cell
    
    if (0==procRank)
      uk0[1+0]    = uk1[1+0] = TempLeft;
    if (procCount-1==procRank)
      uk0[1+JLoc-1] = uk1[1+JLoc-1] = TempRight;

    for (int n=1; n<=N; ++n)
    {
      if (procRank!=0)
        MPI_Send(&uk0[1],1,MPI_DOUBLE,rankLeft,0,MPI_COMM_WORLD);
      if (procRank!=procCount-1)
        MPI_Send(&uk0[JLoc],1,MPI_DOUBLE,rankRight,0,MPI_COMM_WORLD);

      if (procRank!=0)
        MPI_Recv(&uk0[0],1,MPI_DOUBLE,rankLeft,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      if (procRank!=procCount-1)
        MPI_Recv(&uk0[JLoc+1],1,MPI_DOUBLE,rankRight,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    
      // inner calculation
      
      for (int j=2; j<1+JLoc-1; ++j)
      {
        uk1[j] = uk0[j] + dt/(dx*dx) * (uk0[j-1]-2*uk0[j]+uk0[j+1]);
      }

      // boundary calculation
      
      if (procRank!=0)
      {
        int j=1;
        uk1[j] = uk0[j] + dt/(dx*dx) * (uk0[j-1]-2*uk0[j]+uk0[j+1]);
      }
      if (procRank!=procCount-1)
      {
        int j=JLoc;
        uk1[j] = uk0[j] + dt/(dx*dx) * (uk0[j-1]-2*uk0[j]+uk0[j+1]);
      }
    
      double* temp = uk0; uk0 = uk1; uk1 = temp;
    }

    AppendFile("dataP", uk0,procRank*(int)(J/procCount)*dx-dx,1,JLoc);
    
    free(uk1);
    free(uk0);
  }

  MPI_Barrier(MPI_COMM_WORLD);
  
  // parallel non-blocking    
  
  {
    int rankLeft  = procRank-1;
    int rankRight = procRank+1;
    
    int JLoc = J/procCount;
    
    if (procRank==procCount-1) // take into account "last" additional point
      ++JLoc;

    double* uk0 = malloc((JLoc+2)*sizeof(double));
    double* uk1 = malloc((JLoc+2)*sizeof(double));

    for (int i=0; i<JLoc+2; ++i)
      uk0[i] = uk1[i] = TempMid;

    // we usually have an offset of 1 due to the left ghost cell
    
    if (0==procRank)
      uk0[1+0]    = uk1[1+0] = TempLeft;
    if (procCount-1==procRank)
      uk0[1+JLoc-1] = uk1[1+JLoc-1] = TempRight;

    
    MPI_Request reqSendL, reqSendR, reqRecvL, reqRecvR;
    
    for (int n=1; n<=N; ++n)
    {
      // start non-blocking communication
      
      if (procRank!=0)
        MPI_Isend(&uk0[1],1,MPI_DOUBLE,rankLeft,0,MPI_COMM_WORLD, &reqSendL);
      if (procRank!=procCount-1)
        MPI_Isend(&uk0[JLoc],1,MPI_DOUBLE,rankRight,0,MPI_COMM_WORLD, &reqSendR);

      if (procRank!=0)
        MPI_Irecv(&uk0[0],1,MPI_DOUBLE,rankLeft,0,MPI_COMM_WORLD,&reqRecvL);
      if (procRank!=procCount-1)
        MPI_Irecv(&uk0[JLoc+1],1,MPI_DOUBLE,rankRight,0,MPI_COMM_WORLD,&reqRecvR);
    
      // inner calculation
      
      for (int j=2; j<1+JLoc-1; ++j)
      {
        uk1[j] = uk0[j] + dt/(dx*dx) * (uk0[j-1]-2*uk0[j]+uk0[j+1]);
      }

      // wait for communication, but computation is done !!!
      
      if (procRank!=0)
        MPI_Wait(&reqSendL,MPI_STATUS_IGNORE);
      if (procRank!=procCount-1)
        MPI_Wait(&reqSendR,MPI_STATUS_IGNORE);

      if (procRank!=0)
        MPI_Wait(&reqRecvL,MPI_STATUS_IGNORE);
      if (procRank!=procCount-1)
        MPI_Wait(&reqRecvR,MPI_STATUS_IGNORE);
      
      // remaining computation
      
      // boundary calculation
      
      if (procRank!=0)
      {
        int j=1;
        uk1[j] = uk0[j] + dt/(dx*dx) * (uk0[j-1]-2*uk0[j]+uk0[j+1]);
      }
      if (procRank!=procCount-1)
      {
        int j=JLoc;
        uk1[j] = uk0[j] + dt/(dx*dx) * (uk0[j-1]-2*uk0[j]+uk0[j+1]);
      }
    
      double* temp = uk0; uk0 = uk1; uk1 = temp;
    }

    AppendFile("dataNB", uk0,procRank*(int)(J/procCount)*dx-dx,1,JLoc);
    
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
