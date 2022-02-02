PROG=hello

debug:
	gcc -Wall -g -fopenmp ${PROG}.c -o ${PROG}.exe

release:
	gcc -Wall -g -fopenmp -O3 ${PROG}.c -o ${PROG}.exe

run go:
	./${PROG}.exe

clean:
	rm -f ${PROG}.exe ${PROG}.o *~
