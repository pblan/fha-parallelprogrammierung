#include <stdio.h>
#include <stdlib.h>

/*
 * gcc -std=c99 calcPI-serial.c -o calcPI-serial
 * ./calcPI-serial
 */

double f(double x) 
{
  return 1.0 / (1.0 + x*x);
}

int main(int argc, char* argv[]) 
{

  double piNum = 0.0;

  int n = 10000;
  double a=0.0, b=1.0;
  double h = (b-a)/n;
  
  for (int i=0; i<n; ++i)
    piNum += f(a+(i+0.5)*h);

  piNum *= (4*h);
  printf("Pi = %10.8lf \n",piNum);
	
	return EXIT_SUCCESS;
}
