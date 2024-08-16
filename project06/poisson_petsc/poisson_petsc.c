static char help[] = "Solves the poisson equation.\n\n";

/*
  Include "petscksp.h" so that we can use KSP solvers.  Note that this file
  automatically includes:
     petscsys.h       - base PETSc routines   petscvec.h - vectors
     petscmat.h - matrices
     petscis.h     - index sets            petscksp.h - Krylov subspace methods
     petscviewer.h - viewers               petscpc.h  - preconditioners

  Note:  The corresponding uniprocessor example is ex1.c
*/
#include <petscksp.h>
#include <petscviewerhdf5.h>

int main(int argc, char **args)
{
   Vec x, b;        /* approx solution, RHS */
   Mat A;           /* linear system matrix */
   KSP ksp;         /* linear solver context */
   PC pc;           /* preconditioner context */
   PetscInt n = 10; /* size of the grid */
   PetscInt i, N = n * n, col[1], rstart, rend, nlocal;
   PetscScalar twenty = 20.0, value[1], h;

   PetscFunctionBeginUser;
   PetscCall(PetscInitialize(&argc, &args, (char *)0, help));
   PetscCall(PetscOptionsGetInt(NULL, NULL, "-n", &n, NULL));

   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
          Compute the matrix and right-hand-side vector that define
          the linear system, Ax = b.
      - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

   /*
      Create vectors.  Note that we form 1 vector from scratch and
      then duplicate as needed. For this simple case let PETSc decide how
      many elements of the vector are stored on each processor. The second
      argument to VecSetSizes() below causes PETSc to decide.
   */
   PetscCall(VecCreate(PETSC_COMM_WORLD, &x));
   PetscCall(VecSetSizes(x, PETSC_DECIDE, N));
   PetscCall(VecSetFromOptions(x));
   PetscCall(VecDuplicate(x, &b));

   /* Identify the starting and ending mesh points on each
      processor for the interior part of the mesh. We let PETSc decide
      above.
   */
   PetscCall(VecGetOwnershipRange(x, &rstart, &rend));
   PetscCall(VecGetLocalSize(x, &nlocal));

   /*
      Create matrix.  When using MatCreate(), the matrix format can
      be specified at runtime.

      Performance tuning note:  For problems of substantial size,
      preallocation of matrix memory is crucial for attaining good
      performance. See the matrix chapter of the users manual for details.

      We pass in nlocal as the "local" size of the matrix to force it
      to have the same parallel layout as the vector created above.
   */
   PetscCall(MatCreate(PETSC_COMM_WORLD, &A));
   PetscCall(MatSetSizes(A, nlocal, nlocal, N, N));
   PetscCall(MatSetFromOptions(A));
   PetscCall(MatSetUp(A));

   /*
      Assemble matrix -A.

      The linear system is distributed across the processors by
      chunks of contiguous rows, which correspond to contiguous
      sections of the mesh on which the problem is discretized.
      For matrix assembly, each processor contributes entries for
      the part that it owns locally.
   */

   /* 
      Set entries corresponding to the mesh interior, Poisson equation.
   */
   h = 1.0 / (n + 1);
   for (i = rstart; i < rend; i++)
   {
      value[0] = 4.0 / (h * h);
      col[0] = i;
      PetscCall(MatSetValues(A, 1, &i, 1, col, value, INSERT_VALUES));

      if (((i + 1) % n != 0) && (i != N))
      {
         value[0] = -1.0 / (h * h);
         col[0] = i + 1;
         PetscCall(MatSetValues(A, 1, &i, 1, col, value, INSERT_VALUES));
      }

      if ((i % n != 0) && i != 0)
      {
         value[0] = -1.0 / (h * h);
         col[0] = i - 1;
         PetscCall(MatSetValues(A, 1, &i, 1, col, value, INSERT_VALUES));
      }

      if (i + n < N)
      {
         value[0] = -1.0 / (h * h);
         col[0] = i + n;
         PetscCall(MatSetValues(A, 1, &i, 1, col, value, INSERT_VALUES));
      }

      if (i - n >= 0)
      {
         value[0] = -1.0 / (h * h);
         col[0] = i - n;
         PetscCall(MatSetValues(A, 1, &i, 1, col, value, INSERT_VALUES));
      }
   }

   /* 
      Assemble the matrix 
   */
   PetscCall(MatAssemblyBegin(A, MAT_FINAL_ASSEMBLY));
   PetscCall(MatAssemblyEnd(A, MAT_FINAL_ASSEMBLY));

   /*
      Assign value to the b vector, constant source function f
   */
   PetscCall(VecSet(b, twenty));

   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                 Create the linear solver and set various options
      - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
   /*
      Create linear solver context
   */
   PetscCall(KSPCreate(PETSC_COMM_WORLD, &ksp));

   /*
      Set operators. Here the matrix that defines the linear system
      also serves as the preconditioning matrix.
   */
   PetscCall(KSPSetOperators(ksp, A, A));

   /*
      Set linear solver defaults for this problem (optional).
      - By extracting the KSP and PC contexts from the KSP context,
        we can then directly call any KSP and PC routines to set
        various options.
      - The following four statements are optional; all of these
        parameters could alternatively be specified at runtime via
        KSPSetFromOptions();
   */
   PetscCall(KSPGetPC(ksp, &pc));
   PetscCall(PCSetType(pc, PCJACOBI));
   PetscCall(KSPSetTolerances(ksp, 1.e-7, PETSC_DEFAULT, PETSC_DEFAULT, PETSC_DEFAULT));

   /*
     Set runtime options, e.g.,
         -ksp_type <type> -pc_type <type> -ksp_monitor -ksp_rtol <rtol>
     These options will override those specified above as long as
     KSPSetFromOptions() is called _after_ any other customization
     routines.
   */
   PetscCall(KSPSetFromOptions(ksp));

   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                       Solve the linear system
      - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
   /*
      Solve linear system
   */
   PetscCall(KSPSolve(ksp, b, x));

   /*
      View solver info; we could instead use the option -ksp_view to
      print this info to the screen at the conclusion of KSPSolve().
   */
   PetscCall(KSPView(ksp, PETSC_VIEWER_STDOUT_WORLD));

   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                       Check solution and clean up
      - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
   /*
      Print vector x to an output file
   */
   PetscViewer hdf5viewer;
   PetscCall(PetscViewerHDF5Open(PETSC_COMM_WORLD, "solution.h5", FILE_MODE_WRITE, &hdf5viewer));
   PetscCall(VecView(x, hdf5viewer));
   PetscCall(PetscViewerDestroy(&hdf5viewer));

   /*
      Print the matrix A to the console just to check it was assembled correctly
   */
   PetscCall(MatView(A, PETSC_VIEWER_STDOUT_WORLD));

   /*   Free work space.  All PETSc objects should be destroyed when they
      are no longer needed.
   */
   PetscCall(VecDestroy(&x));
   PetscCall(VecDestroy(&b));
   PetscCall(MatDestroy(&A));
   PetscCall(KSPDestroy(&ksp));

   /*
      Always call PetscFinalize() before exiting a program.  This routine
        - finalizes the PETSc libraries as well as MPI
        - provides summary and diagnostic information if certain runtime
          options are chosen (e.g., -log_view).
   */
   PetscCall(PetscFinalize());
   return 0;
}
