const char* dgemm_desc = "Blocked dgemm.";
#include <immintrin.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// 32-byte alignment, since we are using
// AVX2 (4 doubles = 256 bits = 32 bytes)
// Can cause a segfault if not set properly,
// as some operations assume alignment
// (perhaps "vmovapd"?)
#define ALIGNMENT 32

// No. doubles in a AVX2 register
#define VL_FP64 4

// NC just has to be large enough for 
// this use-case, but L3 can fit more
#define NC 1216//1280
#define MC 144
#define KC 244

// 12 by 4 means we can fit micropanels of
// C into registers, since we have YMM[0-15]
// at our disposal and we can pack it
// into MV*NR = 12 YMM registers.
// The other registers we can use to store
// a microtile of A, which is MV = 3 long.
// We then have one (1) remaining register to store
// a broadcast of an element of a microtile of B,
// which we need anyways. So everything fits
// neatly into 16 YMM* registers.
//
// By trying out different MR, NR, 
// it seems that the above considerations/parameters
// indeed lead to good performance.
#define MR 12
#define NR 4
#define MV (MR/VL_FP64)
#define NV (NR/VL_FP64)


#if (NC%NR != 0 || MC%MR != 0 || \
     KC%NR != 0 || MR%VL_FP64 != 0 || \
     NR%VL_FP64 != 0)
#error "Incompatible parameters, i.e. I don't want to deal with these cases."
#endif

// Some macros to make reading
// intrinsics a bit easier
#define vregister __m256d
// Aligned
#define vloada(vreg, mem) vreg = _mm256_load_pd(mem)
// Unaligned
#define vloadu(vreg, mem) vreg = _mm256_loadu_pd(mem)
#define vstoreu(mem, vreg) _mm256_storeu_pd(mem, vreg)

#define vfmadd(vreg1, vreg2, vreg3)\
  vreg1 = _mm256_fmadd_pd(vreg2, \
                          vreg3, \
                          vreg1)

// FMA on v1 with vreg2, vreg3[j] (broadcast)
#define vupdate(vreg1, vreg2, vreg3, j)\
  vfmadd(vreg1, vreg2, _mm256_set1_pd(vreg3[j]))

#define MIN(a, b)(a < b ? a : b)

// Inline, macros are too cumbersome
// to read and debug.
// As for restrict, it probably doesen't hurt 
// to have it.
static inline void __attribute__((always_inline)) 
packB(double* restrict B, double* restrict Bc,
      int row_ceil, int col_ceil, int b_rows, int b_cols){
  // Iterate over the blocks in Bc, the BLOCKS are stored in column-major
  int Bc_offset = 0;
  const int row_ceil_strip = row_ceil - row_ceil%KC;//(row_ceil/KC)*KC;
  const int col_ceil_strip = col_ceil - col_ceil%NC;//(col_ceil/NC)*NC;

  for(int col = 0; col < col_ceil_strip; col += NR){
    // Iterate over the contents of a block, stored in row-major
    // NOTE: But B is still in column-major
    for(int row = 0; row < row_ceil_strip; ++row){
      #pragma GCC ivdep
      for(int loc_col = 0; loc_col < NR; ++loc_col){
        Bc[Bc_offset + row*NR + loc_col] =
          B[row + (col + loc_col)*b_rows];
      }
    }
    Bc_offset += NR*KC;
  }

  if(row_ceil_strip == 0 || col_ceil_strip == 0){
    Bc_offset = 0; 
    for(int col = 0; col < col_ceil; col += NR){
      const int loc_col_ceil = MIN(col_ceil - col, NR);
      // Iterate over the contents of a block, stored in row-major
      // NOTE: But B is still in column-major
      for(int row = 0; row < row_ceil; ++row){
        #pragma GCC unroll 8
        for(int loc_col = 0; loc_col < loc_col_ceil; ++loc_col){
          Bc[Bc_offset + row*NR + loc_col] =
            B[row + (col + loc_col)*b_rows];
        }
      }
      Bc_offset += NR*KC;
    }
  }
  else{
     // Bc_offset = col_ceil_strip*NR*KC; // Already correct from before
    for(int col = col_ceil_strip; col < col_ceil; col += NR){
      const int loc_col_ceil = MIN(col_ceil - col, NR);
      // Iterate over the contents of a block, stored in row-major
      // NOTE: But B is still in column-major
      for(int row = 0; row < row_ceil_strip; ++row){
        #pragma GCC unroll 8
        for(int loc_col = 0; loc_col < loc_col_ceil; ++loc_col){
          Bc[Bc_offset + row*NR + loc_col] =
            B[row + (col + loc_col)*b_rows];
        }
      }
      Bc_offset += NR*KC;
    }
    Bc_offset = 0;
    for(int col = 0; col < col_ceil; col += NR){
      const int loc_col_ceil = MIN(col_ceil - col, NR);
      // Iterate over the contents of a block, stored in row-major
      // NOTE: But B is still in column-major
      for(int row = row_ceil_strip; row < row_ceil; ++row){
        #pragma GCC unroll 8
        for(int loc_col = 0; loc_col < loc_col_ceil; ++loc_col){
          Bc[Bc_offset + row*NR + loc_col] =
            B[row + (col + loc_col)*b_rows];
        }
      }
      Bc_offset += NR*KC;
    }
  }
}

static inline void __attribute__((always_inline)) 
packA(double* restrict A, double* restrict Ac,
      int row_ceil, int col_ceil, int a_rows, int a_cols){
  // Same as for B, but with A, so Ac is "transposed"
  int Ac_offset = 0;
  const int row_ceil_strip = (row_ceil/MC)*MC;
  const int col_ceil_strip = (col_ceil/KC)*KC;

  for(int row = 0; row < row_ceil_strip; row += MR){
    for(int col = 0; col < col_ceil_strip; ++col){
      #pragma GCC ivdep
      for(int loc_row = 0; loc_row < MR; ++loc_row){
        Ac[Ac_offset + col*MR + loc_row] = A[(row + loc_row) + col*a_rows];
      }
    }
    Ac_offset += MR*KC;
  }
  if(row_ceil_strip == 0 || col_ceil_strip == 0){
   Ac_offset = 0;
   for(int row = 0; row < row_ceil; row += MR){
     const int loc_row_ceil = MIN(row_ceil - row, MR);
     for(int col = 0; col < col_ceil; ++col){
       #pragma GCC unroll 8
       for(int loc_row = 0; loc_row < loc_row_ceil; ++loc_row){
         Ac[Ac_offset + col*MR + loc_row] = A[(row + loc_row) + col*a_rows];
       }
     }
     Ac_offset += MR*KC;
   }
  }
  else{
     // Ac offset already correct
    for(int row = row_ceil_strip; row < row_ceil; row += MR){
      const int loc_row_ceil = MIN(row_ceil - row, MR);
      for(int col = 0; col < col_ceil_strip; ++col){
        #pragma GCC unroll 8
        for(int loc_row = 0; loc_row < loc_row_ceil; ++loc_row){
          Ac[Ac_offset + col*MR + loc_row] = A[(row + loc_row) + col*a_rows];
        }
      }
      Ac_offset += MR*KC;
    }
    Ac_offset = 0;
    for(int row = 0; row < row_ceil; row += MR){
      const int loc_row_ceil = MIN(row_ceil - row, MR);
      for(int col = col_ceil_strip; col < col_ceil; ++col){
        #pragma GCC unroll 8
        for(int loc_row = 0; loc_row < loc_row_ceil; ++loc_row){
          Ac[Ac_offset + col*MR + loc_row] = A[(row + loc_row) + col*a_rows];
        }
      }
      Ac_offset += MR*KC;
    }
  }
}

// HAS TO have the correct dimensions,
// otherwise does out-of-bounds accesses
static inline void __attribute__((always_inline))
microkernel_SIMD(double* restrict Ar, 
                 double* restrict Br, 
                 double* restrict C,
                 int C_lda, int pr_ceil) {
#if (MR == 12 && NR == 4)
  // FORCE the values into the registers
  // the compiler has no choice but to only
  // give reads/writes directly to registers
  // in the case of these variables.
  // Avoids having to write assembly
  register vregister
    Cr00 asm("ymm0"), Cr01 asm("ymm1"), Cr02 asm("ymm2"),  Cr03 asm("ymm3"),
    Cr10 asm("ymm4"), Cr11 asm("ymm5"), Cr12 asm("ymm6"),  Cr13 asm("ymm7"),
    Cr20 asm("ymm8"), Cr21 asm("ymm9"), Cr22 asm("ymm10"), Cr23 asm("ymm11");
  register vregister Ar0 asm("ymm12"),
                     Ar1 asm("ymm13"),
                     Ar2 asm("ymm14");
  register vregister Brj asm("ymm15");
  //// Store Cr in registers
  // j = 0
  vloadu(Cr00, &C[0*VL_FP64 + 0*C_lda]);
  vloadu(Cr10, &C[1*VL_FP64 + 0*C_lda]);
  vloadu(Cr20, &C[2*VL_FP64 + 0*C_lda]);
  // j = 1
  vloadu(Cr01, &C[0*VL_FP64 + 1*C_lda]);
  vloadu(Cr11, &C[1*VL_FP64 + 1*C_lda]);
  vloadu(Cr21, &C[2*VL_FP64 + 1*C_lda]);
  // j = 2
  vloadu(Cr02, &C[0*VL_FP64 + 2*C_lda]);
  vloadu(Cr12, &C[1*VL_FP64 + 2*C_lda]);
  vloadu(Cr22, &C[2*VL_FP64 + 2*C_lda]);
  // j = 3
  vloadu(Cr03, &C[0*VL_FP64 + 3*C_lda]);
  vloadu(Cr13, &C[1*VL_FP64 + 3*C_lda]);
  vloadu(Cr23, &C[2*VL_FP64 + 3*C_lda]);

  int a_offset = 0,
      b_offset = 0;
  // Loop over rank-1 updates
  for(int pr = 0; pr < pr_ceil; ++pr){
    // Load Ar
    vloada(Ar0, &Ar[a_offset + 0*VL_FP64]);
    vloada(Ar1, &Ar[a_offset + 1*VL_FP64]);
    vloada(Ar2, &Ar[a_offset + 2*VL_FP64]);
    // j = 0
    // Broadcast element of Br
    Brj = _mm256_set1_pd(Br[b_offset + 0]);
    vfmadd(Cr00, Ar0, Brj);
    vfmadd(Cr10, Ar1, Brj);
    vfmadd(Cr20, Ar2, Brj);
    // j = 1
    // Broadcast element of Br
    Brj = _mm256_set1_pd(Br[b_offset + 1]);
    vfmadd(Cr01, Ar0, Brj);
    vfmadd(Cr11, Ar1, Brj);
    vfmadd(Cr21, Ar2, Brj);
    // j = 2
    // Broadcast element of Br
    Brj = _mm256_set1_pd(Br[b_offset + 2]);
    vfmadd(Cr02, Ar0, Brj);
    vfmadd(Cr12, Ar1, Brj);
    vfmadd(Cr22, Ar2, Brj);
    // j = 2
    // Broadcast element of Br
    Brj = _mm256_set1_pd(Br[b_offset + 3]);
    vfmadd(Cr03, Ar0, Brj);
    vfmadd(Cr13, Ar1, Brj);
    vfmadd(Cr23, Ar2, Brj);

    // Update offsets
    a_offset += MR;
    b_offset += NR;
  }
  //// Load Cr from registers to memory
  // j = 0
  vstoreu(&C[0*VL_FP64 + 0*C_lda], Cr00);
  vstoreu(&C[1*VL_FP64 + 0*C_lda], Cr10);
  vstoreu(&C[2*VL_FP64 + 0*C_lda], Cr20);
  // j = 1
  vstoreu(&C[0*VL_FP64 + 1*C_lda], Cr01);
  vstoreu(&C[1*VL_FP64 + 1*C_lda], Cr11);
  vstoreu(&C[2*VL_FP64 + 1*C_lda], Cr21);
  // j = 2
  vstoreu(&C[0*VL_FP64 + 2*C_lda], Cr02);
  vstoreu(&C[1*VL_FP64 + 2*C_lda], Cr12);
  vstoreu(&C[2*VL_FP64 + 2*C_lda], Cr22);
  // j = 3
  vstoreu(&C[0*VL_FP64 + 3*C_lda], Cr03);
  vstoreu(&C[1*VL_FP64 + 3*C_lda], Cr13);
  vstoreu(&C[2*VL_FP64 + 3*C_lda], Cr23);
// If the size is not the one which was optimized for,
// use a more generic kernel
#else
  vregister Cr[MV][NR], 
            Ar_[MV], Br_[NV];

  // Load a chunk of C into Cr
  for(int j = 0; j < NR; ++j){
    for(int iv = 0; iv < MV; ++iv){
      vloadu(Cr[iv][j], &C[iv*VL_FP64 + j*C_lda]);
    }
  }

  int a_offset = 0,
      b_offset = 0;
  // Loop over rank-1 updates
  for(int pr = 0; pr < pr_ceil; ++pr){
    // Load Ar
    for(int iv = 0; iv < MV; ++iv)
      vloada(Ar_[iv], &Ar[a_offset + iv*VL_FP64]);
    // Load Br AFTER Ar
    for(int jv = 0; jv < NV; ++jv)
      vloada(Br_[jv], &Br[b_offset + jv*VL_FP64]);

    // Execute microkerneh
    for(int iv = 0; iv < MV; ++iv)
      for(int jv = 0; jv < NV; ++jv)
        for(int j = 0; j < VL_FP64; ++j)
          vupdate(Cr[iv][j + jv*VL_FP64], Ar_[iv], Br_[jv], j);

    // Update offsets
    a_offset += MR;
    b_offset += NR;
  }

  // Place the stuff back into C from Cr
  for(int j = 0; j < NR; ++j)
    for(int iv = 0; iv < MV; ++iv)
      vstoreu(&C[iv*VL_FP64 + j*C_lda], Cr[iv][j]);
#endif
}

// To deal with the general case, when we have 
// tiles which don't fit
static inline void __attribute__((always_inline)) 
microkernel_naive(double* restrict Ar, double* restrict Br,
                  double* restrict C, int C_lda, int pr_ceil,
                  int row_ceil, int col_ceil) {
  int a_offset = 0,
      b_offset = 0;
  vregister Cr[MV][NR], 
            Ar_[MV], Br_[NV];

  // Load a chunk of C into Cr
  for(int j = 0; j < col_ceil; ++j){
    int iv;
    for(iv = 0; iv < row_ceil/VL_FP64; ++iv) // MV
      vloadu(Cr[iv][j], &C[iv*VL_FP64 + j*C_lda]);
    
    for(; iv < (row_ceil + (VL_FP64 - 1))/(VL_FP64); ++iv){
      for(int l = 0; l + iv*VL_FP64 < row_ceil; ++l){
          Cr[iv][j][l] = C[l + iv*VL_FP64 + j*C_lda];
      }
    }
  }

  // Loop over rank-1 updates
  for(int pr = 0; pr < pr_ceil; ++pr){
    // Load Ar
    int iv;
    for(iv = 0; iv < row_ceil/VL_FP64; ++iv)
      vloada(Ar_[iv], &Ar[a_offset + iv*VL_FP64]);

    for(; iv < (row_ceil + (VL_FP64 - 1))/(VL_FP64); ++iv)
      for(int l = 0; l + iv*VL_FP64 < row_ceil; ++l)
          Ar_[iv][l] = Ar[a_offset + iv*VL_FP64 + l];

    // Load Br AFTER Ar
    int jv;
    for(jv = 0; jv < col_ceil/VL_FP64; ++jv)
      vloada(Br_[jv], &Br[b_offset + jv*VL_FP64]);

    for(; jv < (col_ceil + (VL_FP64 - 1))/(VL_FP64); ++jv)
      for(int l = 0; l + jv*VL_FP64 < col_ceil; ++l)
        Br_[jv][l] = Br[b_offset + jv*VL_FP64 + l];

    // Execute microkernel
    for(int iv = 0; iv < (row_ceil + (VL_FP64 - 1))/(VL_FP64); ++iv)
      for(int jv = 0; jv < (col_ceil + (VL_FP64 - 1))/(VL_FP64); ++jv)
        for(int j = 0; j + jv*VL_FP64 < col_ceil; ++j)
          vupdate(Cr[iv][j + jv*VL_FP64], Ar_[iv], Br_[jv], j);

    // Update offsets
    a_offset += MR;
    b_offset += NR;
  }
  // Place the stuff back into C from Cr
  for(int j = 0; j < col_ceil; ++j){
    int iv;
    for(iv = 0; iv < row_ceil/VL_FP64; ++iv) // MV
      vstoreu(&C[iv*VL_FP64 + j*C_lda], Cr[iv][j]);
    
    for(; iv < (row_ceil + (VL_FP64 - 1))/(VL_FP64); ++iv){
      for(int l = 0; l + iv*VL_FP64 < row_ceil; ++l){
        C[l + iv*VL_FP64 + j*C_lda] = Cr[iv][j][l];
      }
    }
  }
}

static inline void __attribute__((always_inline)) 
macrokernel(double* restrict Ac, double* restrict Bc,
            double* restrict Cc, 
            int macro_row_ceil, int macro_col_ceil,
            int pr_ceil, int c_rows, int c_cols,
            int ic, int jc){

  const int SIMD_col_ceil = (macro_col_ceil/NR)*NR;
  const int SIMD_row_ceil = (macro_row_ceil/MR)*MR;
  int jr, ir;

  // Compute as much as possible with
  // intrinsics
  for(jr = 0; jr < SIMD_col_ceil; jr += NR){
    // For some reason, this loop benefits
    // a lot from unrolling
    #pragma GCC unroll 32
    for(ir = 0; ir < SIMD_row_ceil; ir += MR){ 
      microkernel_SIMD(&Ac[ir*KC], &Bc[jr*KC], 
                       &Cc[ir + jr*c_rows],
                       c_rows, pr_ceil);
    }
  }

  // Compute the rest with "naive" methods

  // If either of the SIMD_*_ceil is zero, 
  // both loops above are never touched.
  // We have to do everything naively
  if(SIMD_col_ceil == 0 || SIMD_row_ceil == 0){
    for(jr = 0; jr < macro_col_ceil; jr += NR){
      const int clim = MIN(c_cols - jc - jr, NR);
      #pragma GCC unroll 8
      for(ir = 0; ir < macro_row_ceil; ir += MR){ 
        const int rlim = MIN(c_rows - ic - ir, MR);
        microkernel_naive(&Ac[ir*KC], &Bc[jr*KC], 
                  &Cc[ir + jr*c_rows], c_rows, pr_ceil, 
                  rlim, clim);
      }
    }
  }
  else { // Both are non-zero, so fill the remaining parts
    // jr is incremented 0 or 1 times, no unrolling necessary
    for(jr = SIMD_col_ceil; jr < macro_col_ceil; jr += NR){
      const int clim = MIN(c_cols - jc - jr, NR);
      #pragma GCC unroll 8
      for(ir = 0; ir < SIMD_row_ceil; ir += MR){ 
        const int rlim = MIN(c_rows - ic - ir, MR);
        microkernel_naive(&Ac[ir*KC], &Bc[jr*KC], 
                  &Cc[ir + jr*c_rows], c_rows, pr_ceil, 
                  rlim, clim);
      }
    }
    #pragma GCC unroll 8
    for(jr = 0; jr < macro_col_ceil; jr += NR){
      const int clim = MIN(c_cols - jc - jr, NR);
      // ir is incremented 0 or 1 times, no unrolling necessary
      for(ir = SIMD_row_ceil; ir < macro_row_ceil; ir += MR){ 
        const int rlim = MIN(c_rows - ic - ir, MR);
        microkernel_naive(&Ac[ir*KC], &Bc[jr*KC], 
                  &Cc[ir + jr*c_rows], c_rows, pr_ceil, 
                  rlim, clim);
      }
    }
  }
}

void dgemm(int n, int k, int m, 
           double* restrict A,
           double* restrict B,
           double* restrict C){
  // printf("MC: %i\tKC: %i\tNC: %i\n", MC, KC, NC);
  // Allocate on the stack, we have alignment as well
  // as sufficient storage (also, making NC, MC, KC too large will
  // make it complain as it needs to store it all on stack)
  double* Bc = NULL;
  posix_memalign((void**)&Bc, ALIGNMENT, KC*NC*sizeof(double));
  double* Ac = NULL; 
  posix_memalign((void**)&Ac, ALIGNMENT, MC*KC*sizeof(double));

  for(int jc = 0; jc < n; jc+=NC){
    const int macro_col_ceil = MIN(n - jc, NC);
    const int B_col_ceil = macro_col_ceil;
    for(int pc = 0; pc < k; pc+=KC){
      const int pr_ceil = MIN(k - pc, KC);
      const int B_row_ceil = MIN(k - pc, KC);
      const int A_col_ceil = MIN(k - pc, KC);
      packB(&B[pc + jc*k], Bc, B_row_ceil, B_col_ceil, k, n);
      for(int ic = 0; ic < m; ic+=MC){
        const int macro_row_ceil = MIN(m - ic, MC);
        const int A_row_ceil = macro_row_ceil;
        packA(&A[ic + pc*m], Ac, A_row_ceil, A_col_ceil, m, k);
        macrokernel(Ac, Bc, &C[ic + jc*m], 
                    macro_row_ceil, macro_col_ceil,
                    pr_ceil, m, n, ic, jc);
      }
    }
  }

  free(Bc);
  free(Ac);
}

/* This routine performs a dgemm operation
 *
 *  C := C + A * B
 *
 * where A, B, and C are lda-by-lda matrices stored in column-major format.
 * On exit, A and B maintain their input values.
 */
void square_dgemm(int n, double* restrict A,
                         double* restrict B,
                         double* restrict C){
  dgemm(n, n, n, A, B, C);
}
