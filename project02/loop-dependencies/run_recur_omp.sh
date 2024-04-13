#!/bin/bash
#SBATCH --job-name=loop_dep_par      # Job name
#SBATCH --output=loop_dep_par-%j.out # Output file
#SBATCH --error=loop_dep_par-%j.err  # Error file
#SBATCH --ntasks=1                # Number of tasks
#SBATCH --constraint=EPYC_7763    # Select node with CPU
#SBATCH --cpus-per-task=8         # Number of CPUs per task
#SBATCH --mem-per-cpu=3048        # Memory per CPU
#SBATCH --time=00:05:00           # Wall clock time limit

# # Load some modules & list loaded modules
# module load gcc
# module list

# Compile
make clean
make recur_omp

# Create results file
> results_omp.txt

echo "Threads,Parallel RunTime,Final Result Sn,Result ||opt^2_2||" | tee -a results_omp.txt

# Run the program for OMP_NUM_THREADS equal to 1, 2, 4, 8, ..., 64, 128
for ((i=0; i<=7; i++)) do
    OMP_NUM_THREADS=$((2**i)) # Set number of OpenMP threads
    echo "Running with $OMP_NUM_THREADS threads"
    export OMP_NUM_THREADS
    ./recur_omp | tee -a results_omp.txt
    echo "-----------------------------------------"
done
