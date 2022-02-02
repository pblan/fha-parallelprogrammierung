PROG=jacobi

debug:
	gcc -g -fopenmp ${PROG}.c -o ${PROG}.exe -lm 

release:
	gcc -g -fopenmp -Wall -O3 ${PROG}.c -o ${PROG}.exe -lm 

run go: ${PROG}.exe
	OMP_DYNAMIC=false ./${PROG}.exe < input

clean:
	rm -rf *.er *~ ${PROG}.exe ${PROG}.o


${PROG}.exe: debug
