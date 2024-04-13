#include <stdio.h>
#include <stdlib.h>
#include "walltime.h"

void print_list(double *data, int length)
{
  for (int i = 0; i < length; i++)
  {
    printf("%e\t", data[i]);
  }
  printf("\n");
}

void quicksort(double *data, int length)
{

  if (length <= 1)
    return;

  // int tid, nthreads;
  // #pragma omp parallel shared(nthreads, data) private(tid)
  //   {
  //
  //     /* Obtain thread number and number of threads */
  //     tid = omp_get_thread_num();
  // #pragma omp master
  //     {
  //       nthreads = omp_get_num_threads();
  //       // printf("Number of threads = %d\n", nthreads);
  //     }
  //     // printf("Thread %d starting...\n", tid);

  double pivot = data[0];
  double temp;
  int left = 1;
  int right = length - 1;

  do
  {
    while (left < length && data[left] <= pivot)
      left++;
    while (right >= 0 && data[right] >= pivot)
      right--;

    /* swap elements */
    if (left < right)
    {
      temp = data[left];
      data[left] = data[right];
      data[right] = temp;
    }
  } while (left < right);

  if (data[right] < pivot)
  {
    data[0] = data[right];
    data[right] = pivot;
  }

  // #pragma omp task
  // printf("Thread %d initializing right split\n", tid);
  quicksort(data, right);

  // #pragma omp task
  // printf("Thread %d initializing left split\n", tid);
  quicksort(&(data[left]), length - left);
}

int check(double *data, int length)
{
  for (int i = 1; i < length; i++)
  {
    if (data[i] < data[i - 1])
      return 1;
  }
  return 0;
}

int main(int argc, char **argv)
{
  int length;
  double *data;

  length = 10000000;
  if (argc > 1)
    length = atoi(argv[1]);

  data = (double *)malloc(length * sizeof(double));
  if (data == NULL)
  {
    printf("Memory allocation failed\n");
    return 0;
  }

  srand(0);
  for (int i = 0; i < length; i++)
  {
    data[i] = (double)rand() / (double)RAND_MAX;
  }

  double time_start = walltime();
  quicksort(data, length);
  double time = walltime() - time_start;

  printf("Size of dataset: %d, elapsed time[s]: %e \n", length, time);

  if (check(data, length) != 0)
    printf("Quicksort incorrect.\n");

  return 0;
}
