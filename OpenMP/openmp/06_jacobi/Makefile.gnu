PROG=jacobi
MPI_PROCS=1

debug:
	gcc -Wall -g -fopenmp -I/usr/lib64/mpi/gcc/openmpi/include/ ${PROG}.c -o ${PROG}.exe -lm -L/usr/lib64/mpi/gcc/openmpi/lib64/ -lmpi

release:
	gcc -Wall -g -fopenmp -Wall -O3 -I/usr/lib64/mpi/gcc/openmpi/include/ ${PROG}.c -o ${PROG}.exe -lm -L/usr/lib64/mpi/gcc/openmpi/lib64/ -lmpi

run go: ${PROG}.exe
	OMP_DYNAMIC=false mpiexec -n $(MPI_PROCS) ./${PROG}.exe < input

clean:
	rm -rf *.er *~ ${PROG}.exe ${PROG}.o


${PROG}.exe: debug
