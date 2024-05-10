#!/bin/bash
#SBATCH --job-name=mini_app_omp_weak      # Job name   
#SBATCH --output=mini_app_omp_weak-%j.out # Output file
#SBATCH --error=mini_app_omp_weak-%j.err  # Error file 
#SBATCH --ntasks=64               # Number of tasks
#SBATCH --constraint=EPYC_7763    # Select node with CPU
#SBATCH --cpus-per-task=1         # Number of CPUs per task
#SBATCH --mem-per-cpu=1024        # Memory per CPU
#SBATCH --time=03:00:00           # Wall clock time limit

# Load some modules & list loaded modules
module load gcc openmpi

# Compile
# make clean
make

# Create results file
> results_weak.txt
echo "threads,nx,nt,iters_cg,iters_newton,timespent" | tee -a results_weak.txt

# Base resolutions loop
for ((k=0; k<3; k++)) do
    N=$((64*(2**k)))
    echo "Running with $N x $N"

    # MPI processors loop
    for ((i=0; i<4; i++)) do
        P=$((4**i))
        echo "Running with $P MPI processes"
        # Adjust size to maintain proportionality
        n_=$((N*(2**i)))
        
        # Number of runs loop
        for ((j=0; j<50; j++)) do
            mpirun -np $P ./main $n_ 100 0.005 |  grep -i "###" | sed 's/###//g' | sed 's/ //g' | tee -a results_weak.txt
        done
    done
done

# python plot.py