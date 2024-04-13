#include <stdio.h>  /* printf */
#include <stdlib.h> /* atol */
#include <omp.h>

double pi_reduction_calc(long int N)
{
  int i;
  double h, sum, pi, x;

  /* Parallelize with OpenMP using the reduction clause */
  h = 1. / N;
  sum = 0.;

#pragma omp parallel for default(shared) private(i, x) reduction(+ : sum)
  for (i = 0; i < N; ++i)
  {
    x = (i + 0.5) * h;
    sum += 4.0 / (1.0 + x * x);
  }
  pi = sum * h;
  return pi;
}
