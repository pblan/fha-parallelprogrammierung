PROG=for

debug:
	g++ -Wall -g -fopenmp ${PROG}.c -o ${PROG}.exe

release:
	g++ -Wall -g -fopenmp -O3 ${PROG}.c -o ${PROG}.exe

gprof:
	g++ -Wall -pg -fopenmp -O2 ${PROG}.c -o ${PROG}.exe

run go: {PROG}.exe
	./${PROG}.exe

clean:
	rm -rf ${PROG}.exe ${PROG}.o *~ *.er gmon.out gprof.txt

{PROG}.exe: debug

