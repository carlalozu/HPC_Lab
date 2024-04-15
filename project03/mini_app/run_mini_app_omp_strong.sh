#!/bin/bash
#SBATCH --job-name=mini_app_omp_strong      # Job name   
#SBATCH --output=mini_app_omp_strong-%j.out # Output file
#SBATCH --error=mini_app_omp_strong-%j.err  # Error file 
#SBATCH --ntasks=1                # Number of tasks
#SBATCH --constraint=EPYC_7763    # Select node with CPU
#SBATCH --cpus-per-task=16         # Number of CPUs per task
#SBATCH --mem-per-cpu=1024        # Memory per CPU
#SBATCH --time=03:00:00           # Wall clock time limit

# Load some modules & list loaded modules
module load gcc python

# Compile
make clean
make main CXXFLAGS=-fopenmp

# Create results file
# > results_strong.txt
# echo "threads,nx,nt,iters_cg,iters_newton,timespent" | tee -a results_strong.txt

N=$((64*(2**4)))
echo "Running with $N N"
for ((i=0; i<5; i++)) do
    OMP_NUM_THREADS=$((2**i)) # Set number of OpenMP threads
    echo "Running with $OMP_NUM_THREADS threads"
    export OMP_NUM_THREADS
    for ((j=0; j<50; j++)) do
        ./main $N 100 0.005 |  grep -i "###" | sed 's/###//g' | sed 's/ //g' | tee -a results_strong.txt
    done
done

# python plot.py