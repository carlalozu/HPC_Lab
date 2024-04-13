#include <stdio.h>  // printf
#include <stdlib.h> // atol
#include <omp.h>    // OpenMP header

double pi_critical_calc(long int N)
{
  double h, sum = 0.0, pi, x;
  int i;
  h = 1.0 / N;
  /* Parallelize with OpenMP using the critical directive */
#pragma omp parallel shared(h) private(i, x)
  {
    double local_sum = 0.0;
#pragma omp for
    for (i = 0; i < N; ++i)
    {
      x = (i + 0.5) * h;
      local_sum += 4.0 / (1.0 + x * x);
    }
#pragma omp critical
    sum += local_sum;
  } /* end of parallel section */
  pi = sum * h;
  return pi;
}
