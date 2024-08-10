#!/bin/bash
#SBATCH --job-name=slurm_job_two      # Job name    (default: sbatch)
#SBATCH --output=slurm_job_two-%j.out # Output file (default: slurm-%j.out)
#SBATCH --error=slurm_job_two-%j.err  # Error file  (default: slurm-%j.out)
#SBATCH --ntasks=2                    # Number of tasks
#SBATCH --nodes=2                     # Number of nodes
#SBATCH --cpus-per-task=1             # Number of CPUs per task
#SBATCH --mem-per-cpu=1024            # Memory per CPU
#SBATCH --time=00:01:00               # Wall clock time limit
#SBATCH --constraint=EPYC_9654        # Select nodes with certain CPU

# load some modules & list loaded modules
module load gcc
module list

# print CPU model
lscpu | grep "Model name"

# Compile binary
g++ -o hostname hostname.cpp

# run (srun: run job on cluster with provided resources/allocation)
srun hostname
