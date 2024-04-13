#include "walltime.h"
#include <iostream>
#include <random>
#include <vector>
#include <omp.h>

#define VEC_SIZE 1000000000
#define BINS 16
#define CHUNKSIZE 64

int main()
{
  double time_start, time_end;

  // Initialize random number generator
  unsigned int seed = 123;
  float mean = BINS / 2.0;
  float sigma = BINS / 12.0;
  std::default_random_engine generator(seed);
  std::normal_distribution<float> distribution(mean, sigma);

  // Generate random sequence
  // Note: normal distribution is on interval [-inf; inf]
  //       we want [0; BINS-1]
  std::vector<int> vec(VEC_SIZE);
  for (long i = 0; i < VEC_SIZE; ++i)
  {
    vec[i] = int(distribution(generator));
    if (vec[i] < 0)
      vec[i] = 0;
    if (vec[i] > BINS - 1)
      vec[i] = BINS - 1;
  }

  // Initialize histogram: Set all bins to zero
  long dist[BINS] = {0};
  int num_threads = 0;

  // Parallelize the histogram computation
  time_start = walltime(); // Start timing
#pragma omp parallel shared(num_threads, dist)
  {
    num_threads = omp_get_num_threads();
    long private_dist[BINS] = {0}; // Private copy for each thread

#pragma omp for
    for (long i = 0; i < VEC_SIZE; ++i)
    {
      private_dist[vec[i]]++;
    }

    // Combine private_dist arrays into dist
    for (int i = 0; i < BINS; ++i)
    {
#pragma omp atomic
      dist[i] += private_dist[i];
    }
    time_end = walltime(); // End timing
  }
  std::cout << num_threads << "," << time_end - time_start;
  // Write results
  for (int i = 0; i < BINS; ++i)
  {
    std::cout << "," << dist[i];
  }
  std::cout << std::endl;

  return 0;
}
