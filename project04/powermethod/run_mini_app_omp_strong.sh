#!/bin/bash
#SBATCH --job-name=mini_app_omp_strong      # Job name   
#SBATCH --output=mini_app_omp_strong-%j.out # Output file
#SBATCH --error=mini_app_omp_strong-%j.err  # Error file 
#SBATCH --ntasks=64                # Number of tasks
#SBATCH --constraint=EPYC_7763    # Select node with CPU
#SBATCH --cpus-per-task=1        # Number of CPUs per task
#SBATCH --mem-per-cpu=1024        # Memory per CPU
#SBATCH --time=05:00:00           # Wall clock time limit

# Load some modules & list loaded modulessqueue
module load gcc openmpi

# Compile
make clean
make

for ((i=0; i<7; i++)) do
    N=$((2**i)) # Set number of OpenMP threads
    echo "Running with $N threads"
    for ((j=0; j<50; j++)) do
        mpirun -np $N ./powermethod_rows 3 10000 3000 -1e-6 |  grep -i "###" | sed 's/###//g' | sed 's/ //g' | tee -a results_strong.txt
    done
done

# python plot.py