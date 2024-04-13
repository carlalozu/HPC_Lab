#include <stdio.h>  /* printf */
#include <stdlib.h> /* atol */

double pi_serial_calc(long int N)
{
  double h, sum, pi;

  h = 1. / N;
  sum = 0.;
  int i;
  for (i = 0; i < N; ++i)
  {
    double x = (i + 0.5) * h;
    sum += 4.0 / (1.0 + x * x);
  }
  pi = sum * h;

  return pi;
}
