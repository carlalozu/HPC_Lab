#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "walltime.h"
#include <omp.h>

int main(int argc, char *argv[])
{
  int N = 20000000;
  double up = 1.00000001;
  double Sn = 1.00000001;
  int n;

  /* allocate memory for the recursion */
  double *opt = (double *)malloc((N + 1) * sizeof(double));
  if (opt == NULL)
  {
    perror("failed to allocate problem size");
    exit(EXIT_FAILURE);
  }

  double time_start = walltime();

  double dist;
  long nthreads, tid;

#pragma omp parallel private(dist, tid) shared(nthreads) reduction(* : Sn)
  {
    nthreads = omp_get_num_threads();
    tid = omp_get_thread_num();
    dist = pow(up, tid * N / nthreads);
    long i;
    for (i = 0; i < N / nthreads; ++i)
    {
      opt[i + tid * N / nthreads] = Sn * dist;
      Sn *= up;
    }
  }

  printf("Parallel RunTime   :  %f seconds\n", walltime() - time_start);
  printf("Final Result Sn    :  %.17g \n", Sn);

  double temp = 0.0;
  for (n = 0; n <= N; ++n)
  {
    temp += opt[n] * opt[n];
  }
  printf("Result ||opt||^2_2 :  %f\n", temp / (double)N);
  printf("\n");

  return 0;
}
