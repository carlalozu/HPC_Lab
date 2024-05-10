#!/bin/bash
#SBATCH --job-name=mini_app      # Job name   
#SBATCH --output=mini_app-%j.out # Output file
#SBATCH --error=mini_app-%j.err  # Error file 
#SBATCH --ntasks=1                # Number of tasks
#SBATCH --constraint=EPYC_7763    # Select node with CPU
#SBATCH --cpus-per-task=8         # Number of CPUs per task
#SBATCH --mem-per-cpu=1024        # Memory per CPU
#SBATCH --time=00:05:00           # Wall clock time limit

# Load some modules & list loaded modules
module load gcc openmpi python
#salloc --ntasks=4 --constraint=EPYC_7763
# Compile
make clean
make

# Create results file
> output.txt

mpirun ./main 128 100 0.005 | tee -a output.txt
python plot.py