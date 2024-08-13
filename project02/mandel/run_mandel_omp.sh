#!/bin/bash
#SBATCH --job-name=hello_omp      # Job name    (default: sbatch)
#SBATCH --output=hello_omp-%j.out # Output file (default: slurm-%j.out)
#SBATCH --error=hello_omp-%j.err  # Error file  (default: slurm-%j.out)
#SBATCH --ntasks=1                # Number of tasks
#SBATCH --constraint=EPYC_7763    # Select node with CPU
#SBATCH --cpus-per-task=8         # Number of CPUs per task
#SBATCH --mem-per-cpu=1024        # Memory per CPU
#SBATCH --time=01:00:00           # Wall clock time limit

# Load some modules & list loaded modules
module load gcc openmpi libpng/1.6.39-fz4tvmr
module list

# Compile
make clean
make mandel_par

# Create results file
> results_strong.txt
echo "Total time,Total number of iterations,Iterations/second,MFlop/s,Threads" | tee -a results_strong.txt

# Run the program for OMP_NUM_THREADS equal to 1, 2, 4, 8, ..., 64, 128
for ((i=0; i<=7; i++)) do
    OMP_NUM_THREADS=$((2**i)) # Set number of OpenMP threads
    echo "Running with $OMP_NUM_THREADS threads"
    export OMP_NUM_THREADS
    ./mandel_par | grep -E "(Total time|Total number of iterations|Iterations/second|MFlop/s|Number of threads)" | sed 's/.*: *//g' | sed 's/ .*//g' | paste -sd ',' - | tee -a results_strong.txt
done
