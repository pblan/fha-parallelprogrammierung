# Intro
Shared-Memory Parallel Systems
- Implicit data distribution
- Implicit communication
- Different types of shared-memory architectures
- -> OpenMP

SMP (Symmetric Multi Processing)
- Memory access time is uniform on all cores
- Limited scalability

ccNUMA (cache-coherent Non-Uniform Memory Architecture)
- Memory access time is non-uniform
- Scalable

Cache Coherence (cc)
- If there are multiple caches not shared by all cores in the system, the system takes care of the cache coherence.

Distributed-memory Parallel Systems
- Explicit data distribution
- Explicit communication
- Scalable
- Programming via MPI

Processes
- A process is the abstraction of a program in execution
- Different states: Running, Blocking, Waiting
- Each process has its own address-space

Threads
- A thread is a lightweight process
- In difference to a process, a thread shares the address-space with all other threads of the process it belongs to, but has its own stack.
-> Common variables between threads

Process and Thread Scheduling by the OS
- Even on a multi-socket / multi-core system you should not make any assumption which process / thread is executed when an where!

Shared-memory Parallelization
- Memory can be accessed by several threads running on different cores in a multi-socket / multi-core system

Distributed-memory Parallelization
- Each process has its own distinct memory
- Communication via Message Passing

Speedup and Efficiency 
- Time using 1 CPU: T(1)
- Time using p CPUs: T(p)
- Speedup S: S(p) = T(1) / T(p)
- Efficiency E: E(p) = S(p) / p
- Ideal case: T(p) = T(1) / p

Amdahl’s Law
- Describes the influence of the serial part onto scalability (without taking any overhead into account).
- S(p) = T(1) / T(p) = ... = 1 / (f + (1-f) / p)
- f: serial part (0 < f <= 1)
- It is rather easy to scale to a small number of cores, but any parallelization is limited by the serial part of the program!

Granularity
- Parallelization on a High Level (low granularity)
    - Chances of low synchronization / communication cost
    - Danger of load balancing issues
- Parallelization on a Low Level (high granularity)
    - Danger of high synchronization / communication cost
    - Chances of avoiding load balancing issues
- Compute intensive programs may employ multiple levels of parallelization, maybe even with multiple parallelization paradigms (hybrid parallelization).

Issues
- Is your parallelization correct?
- It is harder to debug parallel code than serial code!
- Introduction of overhead by parallelization
- Data Races / Race Conditions
- Deadlocks
- Load Balancing
- Serialization
- Irreproducibility / Different numerical results

Parallelization Overhead
- Overhead introduced by the parallelization:
    - Time to start / end / manage threads
    - Time to send / exchange data
    - Time spent in synchronization of threads / processes
- With parallelization:
    - The total CPU time increases,
    - The Wall time decreases,
    - The System time stays the same.
- Efficient parallelization is about minimizing the overhead introduced by the parallelization itself!

Data Races / Race Conditions
- Data Race: Concurrent access of the same memory location by multiple threads without proper synchronization

Deadlock
- When two or more threads / processes are waiting for another to release a resource in a circular chain, the program appears to "hang"

Load Balancing
- perfect load balancing
    - All threads / processes finish at the same time
- load imbalance
    - Some threads / processes take longer than others
    - But: All threads / processes have to wait for the slowest thread / process, which is thus limiting the scalability

Serialization
- Serialization: When threads / processes wait "too much"
    - Limited scalability, if at all

# MPI
