#!/bin/bash
#SBATCH --job-name=mini_app_omp_weak      # Job name   
#SBATCH --output=mini_app_omp_weak-%j.out # Output file
#SBATCH --error=mini_app_omp_weak-%j.err  # Error file 
#SBATCH --ntasks=64                # Number of tasks
#SBATCH --constraint=EPYC_7763    # Select node with CPU
#SBATCH --cpus-per-task=1         # Number of CPUs per task
#SBATCH --mem-per-cpu=1024        # Memory per CPU
#SBATCH --time=05:00:00           # Wall clock time limit

# Load some modules & list loaded modules
module load gcc openmpi

# Compile
make clean
make

# Create results file
# > results_weak.txt
# echo "threads,nx,nt,iters_cg,iters_newton,timespent" | tee -a results_weak.txt

for ((i=0; i<7; i++)) do
    p=$((2**i)) # Set number of OpenMP threads
    echo "Running with $p threads"
    for ((j=0; j<50; j++)) do
        # Do it fifty times for a more reliable
        N=$((2**(i/2)*1500))
        mpirun -np $p ./powermethod_rows 3 $N 3000 -1e-6 |  grep -i "###" | sed 's/###//g' | sed 's/ //g' | tee -a results_strong.txt
    done
done
# python plot.py