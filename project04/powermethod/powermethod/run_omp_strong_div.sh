#!/bin/bash
#SBATCH --job-name=omp_strong_div      # Job name   
#SBATCH --output=omp_strong_div-%j.out # Output file
#SBATCH --error=omp_strong_div-%j.err  # Error file 
#SBATCH --nodes=64                 # Number of nodes
#SBATCH --ntasks=64                # Number of tasks
#SBATCH --ntasks-per-node=1        # Number of tasks per node
#SBATCH --constraint=EPYC_7763     # Select node with CPU
#SBATCH --cpus-per-task=1          # Number of CPUs per task
#SBATCH --mem-per-cpu=1024         # Memory per CPU
#SBATCH --time=05:00:00            # Wall clock time limit

# Load some modules & list loaded modules
module load gcc openmpi

# Compile
make clean
make

# Create results file
# > results_strong.txt
# echo "threads,nx,nt,iters_cg,iters_newton,timespent" | tee -a results_strong.txt

for ((i=4; i<7; i++)) do
    N=$((2**i)) # Set number of OpenMP threads
    echo "Running with $N threads"
    for ((j=0; j<50; j++)) do
        mpirun -np $N ./powermethod_rows 3 10000 3000 -1e-6 |  grep -i "###" | sed 's/###//g' | sed 's/ //g' | tee -a results_strong.txt
    done
done

# python plot.py