const char *dgemm_desc = "Blocked dgemm.";
#include <math.h>
#include <omp.h>

/* This routine performs a dgemm operation
 *
 *  C := C + A * B
 *
 * where A, B, and C are lda-by-lda matrices stored in column-major format.
 * On exit, A and B maintain their input values.
 */
void square_dgemm(int n, double *A, double *B, double *C)
{
  // TODO: Implement the blocking optimization
  //       (The following is a placeholder naive three-loop dgemm)

  const unsigned int L1 = 1 << 15;                                   // L1 cache size 52 Kb
  unsigned int n_ = (unsigned int)sqrt(L1 / (3.0 * sizeof(double))); // block size

// Suggestions for pragma directives:
#pragma omp parallel for collapse(3) schedule(static)
  for (unsigned int j = 0; j < n; j += n_)
  {
    for (unsigned int k = 0; k < n; k += n_)
    {
      for (unsigned int i = 0; i < n; i += n_)
      {
// Macro-kernel
#pragma omp simd
        for (unsigned int jj = j; jj < (j + n_ > n ? n : j + n_); ++jj)
        {
          for (unsigned int kk = k; kk < (k + n_ > n ? n : k + n_); ++kk)
          {
            double b = B[kk + jj * n];
#pragma omp simd
            for (unsigned int ii = i; ii < (i + n_ > n ? n : i + n_); ++ii)
            {
              C[ii + jj * n] += A[ii + kk * n] * b;
            }
          }
        }
      }
    }
  }
}
