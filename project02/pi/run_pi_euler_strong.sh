#!/bin/bash
#SBATCH --job-name=run_pi_euler         # Job name
#SBATCH --output=run_pi_euler-%j.out    # Output file
#SBATCH --error=run_pi_euler-%j.err     # Error file
#SBATCH --ntasks=1                      # Number of tasks
#SBATCH --constraint=EPYC_7763          # Select node with CPU
#SBATCH --cpus-per-task=128             # Number of CPUs per task
#SBATCH --mem-per-cpu=1024              # Memory per CPU
#SBATCH --time=00:05:00                 # Wall clock time limit

# Load some modules & list loaded modules
module load gcc openmpi
module list

# Compile
make clean
make

# Create results file and header
> results_strong.txt
echo "Calculation,Pi_value,Time,Threads,Speedup,Size" | tee -a results_strong.txt

# Run the program for OMP_NUM_THREADS equal to 1, 2, 4, 8, ..., 64, 128
for ((i=0; i<=7; i++)) do
    OMP_NUM_THREADS=$((2**i)) # Set number of OpenMP threads
    SIZE_VEC=$((1000000))
    echo "Running with $OMP_NUM_THREADS threads"
    export OMP_NUM_THREADS
    export SIZE_VEC
    ./walltime | tee -a results_strong.txt
    echo "-----------------------------------------"
done
