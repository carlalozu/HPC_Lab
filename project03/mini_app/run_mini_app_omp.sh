#!/bin/bash
#SBATCH --job-name=mini_app_omp      # Job name   
#SBATCH --output=mini_app_omp-%j.out # Output file
#SBATCH --error=mini_app_omp-%j.err  # Error file 
#SBATCH --ntasks=1                # Number of tasks
#SBATCH --constraint=EPYC_7763    # Select node with CPU
#SBATCH --cpus-per-task=8         # Number of CPUs per task
#SBATCH --mem-per-cpu=1024        # Memory per CPU
#SBATCH --time=00:05:00           # Wall clock time limit

# Load some modules & list loaded modules
module load gcc python

# Compile
make clean
make main CXXFLAGS=-fopenmp

# Create results file
> output.txt

for ((i=0; i<=7; i++)) do
    OMP_NUM_THREADS=$((2**i)) # Set number of OpenMP threads
    echo "Running with $OMP_NUM_THREADS threads"
    export OMP_NUM_THREADS
    ./main_omp 128 100 0.005 | tee -a output.txt
done
python plot.py