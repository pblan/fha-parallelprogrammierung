PROG=pi

debug:
	gcc -Wall -g -fopenmp -lm ${PROG}.c -o ${PROG}.exe

release:
	gcc -Wall -g -fopenmp -O3 -lm ${PROG}.c -o ${PROG}.exe

run go: {PROG}.exe
	./${PROG}.exe

clean:
	rm -f ${PROG}.exe ${PROG}.o *~

{PROG}.exe: debug
