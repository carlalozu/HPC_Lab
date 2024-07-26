#!/bin/bash
#SBATCH --job-name=poisson        # Job name   
#SBATCH --output=poisson-%j.out   # Output file
#SBATCH --error=poisson-%j.err    # Error file 
#SBATCH --ntasks=16                # Number of tasks
#SBATCH --constraint=EPYC_7763    # Select node with CPU
#SBATCH --cpus-per-task=1         # Number of CPUs per task
#SBATCH --mem-per-cpu=1024        # Memory per CPU
#SBATCH --time=01:00:00           # Wall clock time limit

# Compile
make clean
make poisson_petsc

# Create results file
mpiexec -n 8 ./poisson_petsc -ksp_monitor_short -ksp_gmres_cgs_refinement_type refine_always