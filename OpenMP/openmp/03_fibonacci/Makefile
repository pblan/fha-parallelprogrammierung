PROG=fibonacci

debug:
	gcc -g -fopenmp ${PROG}.c -o ${PROG}.exe

release:
	gcc -g -fopenmp -O3 ${PROG}.c -o ${PROG}.exe

run go: {PROG}.exe
	./${PROG}.exe < input

clean:
	rm -rf *.er *~ ${PROG}.exe ${PROG}.o

{PROG}.exe: debug
