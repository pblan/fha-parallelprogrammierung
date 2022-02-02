#include <iostream>
#include <cstdlib>
#include <queue>

#include "mpi.h"

/*
 * mpicxx masterworkermerge.cc -o masterworkermerge
 * mpiexec -np 2 masterworkermerge
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

// output an int-queue, do not overload operator<< here
template <typename T>
void printQueue(const char* pc, std::queue<T>& q)
{
  std::cout << pc << ", size " << q.size() << ": ";
  for (int i=0; i<q.size(); ++i)
  {
    T t = q.front();
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
  std::queue<int*> tasks; // set of tsks to do
  int k=0;
	int* array = NULL;
  if (0==procRank)
  {
    // create processes avail.
    for (int i=1; i<procCount; ++i)
      procs.push(i);
    k=10;
	  array = (int*)malloc((k+1)*sizeof(int));

    array[0] = k;
    initArray(array+1,k,10);
    printArray(array,k+1,0);
    
    tasks.push(array);
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
        int* pdata = tasks.front(); // for this task
        int size = pdata[0];
        tasks.pop();

        std::cout << "[0] start task for proc " << proc << ", data " << "" << std::endl;
        data=0;
        MPI_Ssend(pdata+1,size,MPI_INT,proc,tagStartWork,MPI_COMM_WORLD);
      }
      else // idle or no free resources ...
      {
        printQueue("[0] procs",procs);
        printQueue("[0] tasks",tasks);
        
        sleep(1);
      }
    }
  } // while

 	MPI_Finalize();
	
	return EXIT_SUCCESS;
}
