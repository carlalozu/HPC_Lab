#!/bin/bash
#SBATCH --job-name=loop_dep_seq      # Job name
#SBATCH --output=loop_dep_seq-%j.out # Output file
#SBATCH --error=loop_dep_seq-%j.err  # Error file
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
make recur_seq 

# Create results file
> results_seq.txt

# Run the program
./recur_seq  | tee -a results_seq.txt
