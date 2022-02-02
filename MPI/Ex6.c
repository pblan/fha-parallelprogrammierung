#include "mpi.h"
#include <stdio.h>

int main(int argc, char** argv) 
{
    int procRank, procCount;
    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &procCount);
    MPI_Comm_rank(MPI_COMM_WORLD, &procRank);
    
    int msg;
    if(procRank == 0) 
    {
        
        MPI_Status status;

        for(int i = 1; i < procCount; i++) 
        {
            MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);   

            printf("Received %i from proc. %i\n", msg, status.MPI_SOURCE);     
        }
    }
    else if(procRank == 1)
    {
        msg = 42;
        MPI_Send(&msg, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
        MPI_Send(&msg, 1, MPI_INT, 3, 0, MPI_COMM_WORLD);

        MPI_Send(&msg, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    } 
    else 
    {
        //MPI_Recv(&msg, 1, MPI_INT, procRank / 2, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        if(2*procRank < procCount)
        {
            MPI_Send(&msg, 1, MPI_INT, 2*procRank, 0, MPI_COMM_WORLD);
        }

        if(2*procRank + 1 < procCount)
        {
            MPI_Send(&msg, 1, MPI_INT, 2*procRank + 1, 0, MPI_COMM_WORLD);
        }

        MPI_Send(&msg, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}