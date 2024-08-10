#include "matrix_multiplication.hpp"

// Second implementation trying to use so cache properties
void mm2(matrix_t const &A,
         matrix_t const &B,
         matrix_t &C,
         std::size_t N) noexcept
{
  constexpr std::size_t L1 = 1 << 15; // adapt to your L1 cache size
  std::size_t n = std::sqrt(L1 / (3.0 * sizeof(double))); // block size

  for (std::size_t j = 0; j < N; j += n) {
    for (std::size_t k = 0; k < N; k += n) {
      for (std::size_t i = 0; i < N; i += n) {
        // Macro-kernel
        for (std::size_t jj = j; jj < std::min(j + n, N); ++jj) {
          for (std::size_t kk = k; kk < std::min(k + n, N); ++kk) {
            auto b = B[kk + jj * N];
            for (std::size_t ii = i; ii < std::min(i + n, N); ++ii) {
              C[ii + jj * N] += A[ii + kk * N] * b;
            }
          }
        }
      }
    }
  }
}
