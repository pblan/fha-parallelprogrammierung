/******************************
* Prime number seeker         *
* by Alexander Koch           *
* A.Koch@rz.rwth-aachen.de    *
*                             *
* This mpi program seeks the  *
* next prime number starting  *
* from given number           *
******************************/

//Compile with -lm at the end

//This solution is different to the loadbalancing example from the lessen. Here, we do not have the
//initial send and the final stop call for all processes.

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "mpi.h"

/*
 * mpicc -std=c99 prim-par.c -o prim-par
 * mpiexec -np 1 prim-par
 */


#define TAG_WAITINGFORWORK 1
#define TAG_FOUNDPRIM 2
#define TAG_ISPRIM 3
#define TAG_EXIT 9

int isPrim(const int num) {
	//Very naive approach - but it should scale good
	int i;
	for(i=2;i<sqrt((double)num);i++){
		if(num%i==0) return 0;
	}
	return 1;
}

int main (int argc, char** argv) {
    /************************************************************************************
     *                         I N I T I A L I Z A T I O N                              *
     ************************************************************************************/
    int iMyRank, iNumProcs, iCurValue, iPrim, iWorkersLeft, tmp, iStartValue, iPrimFound;
	MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &iMyRank);
    MPI_Comm_size(MPI_COMM_WORLD, &iNumProcs);

    /************************************************************************************
     *                  M A S T E R   &   W O R K E R   P A R T                         *
     ************************************************************************************/
		iPrimFound = 0;
	if(iMyRank == 0) {
		printf("Start to search for prim from: "); fflush(stdout);
		scanf("%i",&iStartValue);

		iCurValue = iStartValue;
		iPrim = 0;
		iWorkersLeft = iNumProcs-1;
		iPrimFound = 0;

		do {
			MPI_Recv(&tmp,1,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
      
      printf("recv tmp %d \n",tmp);

			switch (status.MPI_TAG) {
				//Worker is waiting for work ... 
				case TAG_WAITINGFORWORK:
					if(iPrimFound) {
						//We already have a prime ... sending command to exit
						MPI_Send(&tmp,1,MPI_INT,status.MPI_SOURCE,TAG_EXIT,MPI_COMM_WORLD);
						iWorkersLeft--;
					}
					else {
						//Sending next value to check to worker
						printf("work to do: %i, worker: %i\n",iCurValue,status.MPI_SOURCE);
						MPI_Send(&iCurValue,1,MPI_INT,status.MPI_SOURCE,TAG_ISPRIM,MPI_COMM_WORLD);
						iCurValue++;
					}
				break;

				case TAG_FOUNDPRIM:
					printf("Prime found: %i\n",tmp);

					//Another process has already found a prime - so let's check which one is smaller 
					if(iPrimFound == 1) {
						if(tmp<iPrim) iPrim=tmp;
					}
					else {
						iPrim = tmp;
						iPrimFound = 1;
					}
					//The worker which found the prime should has already stopped working.
					//We can reduce the number of workers left.
					iWorkersLeft--;
				break;
			}
		}while(iWorkersLeft > 0); //Distribute work until no worker is left
		printf("Smallest prime found: %i\n",iPrim);
	}
	else {
		do {
			//Worker is waiting for work
      printf("work %d idle \n",iMyRank); fflush(stdout);
			MPI_Send(&iCurValue,1,MPI_INT,0,TAG_WAITINGFORWORK,MPI_COMM_WORLD);

			//Recieve the work from the master process
			MPI_Recv(&iCurValue,1,MPI_INT,0,MPI_ANY_TAG,MPI_COMM_WORLD,&status);

      printf("recv work %d \n",iCurValue);

			switch (status.MPI_TAG) {
				//We have to check if the value is a prime
				case TAG_ISPRIM:
          printf("check prim %d \n",iCurValue);
					if(isPrim(iCurValue)) {
            printf("is prim %d \n",iCurValue);
						//It's a prime!
						MPI_Send(&iCurValue,1,MPI_INT,0,TAG_FOUNDPRIM,MPI_COMM_WORLD);
						iPrimFound=1; //Nothing left to do ... we exit after that
					}
          else
            printf("is not prim %d \n",iCurValue);
				break;
					
				//Master says ... exit
				case TAG_EXIT:
					iPrimFound=1;
				break;
			}
      printf("loop %d, found %d  \n",iMyRank,iPrimFound); fflush(stdout);
		}while(!iPrimFound);
    printf("endloop %d \n",iMyRank); fflush(stdout);
	}
	
    /************************************************************************************
     *                            C L E A N    U P                                      *
     ************************************************************************************/
    MPI_Finalize();
    return EXIT_SUCCESS;
}
