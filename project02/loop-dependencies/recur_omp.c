#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "walltime.h"
#include <omp.h>

int main(int argc, char *argv[])
{
  int N = 2000000000;
  double up = 1.00000001;
  double Sn = 1.00000001;
  int n, i;

  /* allocate memory for the recursion */
  double *opt = (double *)malloc((N + 1) * sizeof(double));
  if (opt == NULL)
  {
    perror("failed to allocate problem size");
    exit(EXIT_FAILURE);
  }

  double time_start = walltime();
#pragma omp parallel shared(opt, N, up) private(n, i)
  {
    i = 0;
#pragma omp for firstprivate(Sn) lastprivate(Sn)
    for (n = 0; n <= N; ++n)
    {
      if (i == 0)
        Sn = Sn * pow(up, n);
      opt[n] = Sn;
      Sn *= up;
      i += 1;
    }
  }
  double final_time = walltime() - time_start;

  printf("%d,", omp_get_max_threads());
  printf("%f,", final_time);

  double temp = 0.0;
  for (n = 0; n <= N; ++n)
  {
    temp += opt[n] * opt[n];
  }

  printf("%.17g,", Sn);
  printf("%f\n", temp / (double)N);

  return 0;
}
