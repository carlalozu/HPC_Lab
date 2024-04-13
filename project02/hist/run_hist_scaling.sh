#!/bin/bash
#SBATCH --job-name=hello_omp      # Job name    (default: sbatch)
#SBATCH --output=hello_omp-%j.out # Output file (default: slurm-%j.out)
#SBATCH --error=hello_omp-%j.err  # Error file  (default: slurm-%j.out)
#SBATCH --ntasks=1                # Number of tasks
#SBATCH --constraint=EPYC_7763    # Select node with CPU
#SBATCH --cpus-per-task=128       # Number of CPUs per task
#SBATCH --mem-per-cpu=1024        # Memory per CPU
#SBATCH --time=01:00:00           # Wall clock time limit

# Load some modules & list loaded modules
module load gcc openmpi
module list

# Compile
make clean
make

echo "Parallel"

# Write header
echo -n  "Threads,Time" | tee -a results.txt
for ((i = 0; i < 16; ++i))
do
  echo -n ",$i" | tee -a results.txt
done


# Run the program for OMP_NUM_THREADS equal to 1, 2, 4, 8, ..., 64, 128
for ((i=0; i<=7; i++))
do
  OMP_NUM_THREADS=$((2**i))
  echo "Running with n_threads: $OMP_NUM_THREADS"
  export OMP_NUM_THREADS
  ./hist_omp | tee -a results.txt
  echo "-----------------------------------------"
done
