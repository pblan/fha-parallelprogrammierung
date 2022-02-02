PROG=PrimeOpenMP

debug:
	g++ -Wall -g -fopenmp ${PROG}.c -o ${PROG}.exe

release:
	g++ -Wall -g -fopenmp -O3 ${PROG}.c -o ${PROG}.exe

run go: {PROG}.exe
	OMP_DYNAMIC=false ./${PROG}.exe 0 10000000

clean:
	rm -rf *.er *~ ${PROG}.exe ${PROG}.o

{PROG}.exe: debug
