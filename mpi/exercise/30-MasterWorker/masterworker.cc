#include <iostream>
#include <cstdlib>
#include <queue>

#include "mpi.h"

/*
 * mpicxx masterworker.cc -o masterworker
 * mpiexec -np 4 masterworker
 */

// output an int-queue, do not overload operator<< here
void printQueue(const char* pc, std::queue<int>& q)
{
  std::cout << pc << ", size " << q.size() << ": ";
  for (int i=0; i<q.size(); ++i)
  {
    int t = q.front();
    q.pop();
    std::cout << " " << t;
    q.push(t);
  }
  std::cout << " " << std::endl;
}

int main(int argc, char* argv[]) 
{
	int procRank,procCount;

	MPI_Init(&argc,&argv);

	MPI_Comm_size(MPI_COMM_WORLD,&procCount);
	MPI_Comm_rank(MPI_COMM_WORLD,&procRank);
  
  std::cout << "Start[" << procRank << "]/[" << procCount << "]" << std::endl;
  
  enum {
    tagStartWork = 1,
    tagFinishedWork,
  };

  std::queue<int> procs; // set of avail. processes	
  std::queue<int> tasks; // set of tsks to do
  if (0==procRank)
  {
    // create processes avail.
    for (int i=1; i<procCount; ++i)
      procs.push(i);
  }

  bool stop = false;
  while (!stop) 
  {
    MPI_Status status;
    int flag=-1, data=-1, proc;
    MPI_Iprobe(MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&flag,&status);
    
    if (0!=flag) // you got mail ...
    {
      switch(status.MPI_TAG) 
      {
        case tagFinishedWork: // for master only
        {
          MPI_Recv(&data,1,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
          std::cout << "[0] got result from proc [" << status.MPI_SOURCE << "], result " << data << std::endl;
          procs.push(status.MPI_SOURCE);
        }
        break;
        case tagStartWork: // for workers only
        {
          MPI_Recv(&data,1,MPI_INT,0,MPI_ANY_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
          std::cout << "[" << procRank << "] got task, data " << data << std::endl;
          data *= 2;
          sleep(3);
          std::cout << "[" << procRank << "] task finished, result " << data << std::endl;
          MPI_Ssend(&data,1,MPI_INT,0,tagFinishedWork,MPI_COMM_WORLD);
        }
        break;
      }
    }
    else if (0==procRank)// no mail
    {
      // if there is something to do and we have procs for that
      if (tasks.size()>0 && procs.size()>0)
      {
        proc = procs.front(); // use this proc
        procs.pop();
        data = tasks.front(); // for this task
        tasks.pop();

        std::cout << "[0] start task for proc " << proc << ", data " << data << std::endl;
        MPI_Ssend(&data,1,MPI_INT,proc,tagStartWork,MPI_COMM_WORLD);
      }
      else // idle or no free resources ...
      {
        printQueue("[0] procs",procs);
        printQueue("[0] tasks",tasks);

        sleep(1);
      
        if (0==(rand() % 3)) 
        {
          data = rand()%10;
          std::cout << "[0] create task, data " << data <<std::endl;
          tasks.push(data);
        }
      }
    }
  } // while

 	MPI_Finalize();
	
	return EXIT_SUCCESS;
}
