#include <stdio.h>
#include <stdlib.h>

#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <omp.h>

#include "consts.h"
#include "pngwriter.h"
#include "walltime.h"

int main(int argc, char **argv)
{
       png_data *pPng = png_create(IMAGE_WIDTH, IMAGE_HEIGHT);

       double x, y, x2, y2, cx, cy;

       double fDeltaX = (MAX_X - MIN_X) / (double)IMAGE_WIDTH;
       double fDeltaY = (MAX_Y - MIN_Y) / (double)IMAGE_HEIGHT;

       // printf("Image size: %ld x %ld = %ld Pixels\n",
       //        (long)IMAGE_WIDTH, (long)IMAGE_HEIGHT,
       //        (long)(IMAGE_WIDTH * IMAGE_HEIGHT));
       long nTotalIterationsCount = 0, i, j, n;
       int c;
       int num_threads = 0;

       double time_start = walltime();
// do the calculation
#pragma omp parallel shared(pPng, fDeltaX, fDeltaY, nTotalIterationsCount, num_threads) private(j, i, x, y, y2, x2, cx, cy, n, c)
       {
              // Get the number of threads
              num_threads = omp_get_num_threads();
#pragma omp for
              for (j = 0; j < IMAGE_HEIGHT; j++)
              {
                     cy = fDeltaY * j + MIN_Y;
                     for (i = 0; i < IMAGE_WIDTH; i++)
                     {
                            cx = fDeltaX * i + MIN_X;
                            x = 0;
                            y = 0;
                            x2 = 0;
                            y2 = 0;
                            n = 0;
                            // compute the orbit z, f(z), f^2(z), f^3(z), ...
                            // count the iterations until the orbit leaves the circle |z|=2.
                            // stop if the number of iterations exceeds the bound MAX_ITERS.
                            // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
                            while (((x2 + y2) <= 4) && (n < MAX_ITERS))
                            {
                                   y = 2 * x * y + cy;
                                   x = x2 - y2 + cx;
                                   x2 = x * x;
                                   y2 = y * y;
                                   n++;
                            }
                            // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
                            // n indicates if the point belongs to the mandelbrot set
                            // plot the number of iterations at point (i, j)
#pragma omp critical
                            c = ((long)n * 255) / MAX_ITERS;
                            png_plot(pPng, i, j, c, c, c);
                            nTotalIterationsCount += n;
                     }
              }
       }
       double time_end = walltime();
       // print benchmark data
       printf("Total time:                 %g seconds\n",
              (time_end - time_start));
       printf("Image size:                 %ldx%ld=%ld Pixels\n",
              (long)IMAGE_WIDTH, (long)IMAGE_HEIGHT,
              (long)(IMAGE_WIDTH * IMAGE_HEIGHT));
       printf("Total number of iterations: %ld\n", nTotalIterationsCount);
       printf("Avg. time per pixel:        %g seconds\n",
              (time_end - time_start) / (double)(IMAGE_WIDTH * IMAGE_HEIGHT));
       printf("Avg. time per iteration:    %g seconds\n",
              (time_end - time_start) / (double)nTotalIterationsCount);
       printf("Iterations/second:          %g\n",
              nTotalIterationsCount / (time_end - time_start));
       // assume there are 8 floating point operations per iteration
       printf("MFlop/s:                    %g\n",
              nTotalIterationsCount * 8.0 / (time_end - time_start) * 1.e-6);
       printf("Number of threads:          %i\n", num_threads);

       png_write(pPng, "mandel.png");
       return 0;
}
