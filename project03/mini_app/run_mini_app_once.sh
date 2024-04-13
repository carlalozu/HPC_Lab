srun --time=01:00:00 --constraint=EPYC_7763 --pty bash
make clean
make main CXXFLAGS=-fopenmp
export OMP_NUM_THREADS=4
./main 128 100 0.005