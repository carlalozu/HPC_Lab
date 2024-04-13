#!/bin/bash
#SBATCH --job-name=slurm_job_stream_I      # Job name
#SBATCH --output=slurm_job_stream_I-%j.out # Output file
#SBATCH --error=slurm_job_stream_I-%j.err  # Error file
#SBATCH --ntasks=1                    # Number of tasks
#SBATCH --cpus-per-task=1             # Number of CPUs per task
#SBATCH --mem-per-cpu=1024            # Memory per CPU
#SBATCH --time=00:01:00               # Wall clock time limit
#SBATCH --constraint=EPYC_7H12        # CPU model

# load a compiler module
module load gcc

# compile (DSTREAM_ARRAY_SIZE change depending on 4* the size of the last cache level)
gcc -O3 -march=native -DSTREAM_TYPE=double -DSTREAM_ARRAY_SIZE=8500000 \
    -DNTIMES=20 stream.c -o stream_c_I.exe

# submit to queue and run (require enough memory!)
sbatch --mem-per-cpu=2G --wrap "./stream_c_I.exe"