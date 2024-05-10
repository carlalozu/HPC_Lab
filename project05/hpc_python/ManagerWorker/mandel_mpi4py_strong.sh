#!/bin/bash
#SBATCH --job-name=mandel_mpi4py_strong      # Job name   
#SBATCH --output=mandel_mpi4py_strong-%j.out # Output file
#SBATCH --error=mandel_mpi4py_strong-%j.err  # Error file 
#SBATCH --ntasks=32               # Number of tasks
#SBATCH --constraint=EPYC_7763    # Select node with CPU
#SBATCH --cpus-per-task=1         # Number of CPUs per task
#SBATCH --mem-per-cpu=1024        # Memory per CPU
#SBATCH --time=03:00:00           # Wall clock time limit

# Load some modules & list loaded modules
module load gcc openmpi python

# Open venv
python -m venv project05-env

# Create results file
> results_strong.txt
echo "nx,ny,ntasks,processes,timespent,TasksDoneByWorker" | tee -a results_strong.txt
# Size loop
for ((k=0; k<2; k++)) do
    N=$((50*(2**k)))
    echo "Running with $N tasks"

    # MPI processors loop
    for ((i=1; i<6; i++)) do
        P=$((2**i))
        echo "Running with $P MPI processes"

        # Number of runs loop
        for ((j=0; j<50; j++)) do
            mpirun -np $P python3 manager_worker.py 4001 4001 $N  |  grep -i "###" | sed 's/###//g'| tee -a results_strong.txt
        done
    done
done

# python plot.py