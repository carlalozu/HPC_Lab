#!/bin/bash
#SBATCH --job-name=slurm_job_eulerVII_I      # Job name    (default: sbatch)
#SBATCH --output=slurm_job_eulerVII_I-%j.out # Output file (default: slurm-%j.out)
#SBATCH --error=slurm_job_eulerVII_I-%j.err  # Error file  (default: slurm-%j.out)
#SBATCH --ntasks=1                    # Number of tasks
#SBATCH --cpus-per-task=1             # Number of CPUs per task
#SBATCH --mem-per-cpu=1024            # Memory per CPU
#SBATCH --time=00:01:00               # Wall clock time limit
#SBATCH --constraint=EPYC_7H12

# no modules needed

# determine CPU architecture
lscpu > lscpu.txt

# get memory hierarchy
cat /proc/meminfo > meminfo.txt

# get more info and save to text file and fig
hwloc-ls --whole-system --no-io > hwloc.txt
hwloc-ls --whole-system --no-io -f --of fig EPYC_7H12.fig

# run (srun: run job on cluster with provided resources/allocation)
srun hostname