from mpi4py import MPI
import numpy as np

# get comm, size, rank & host name
comm = MPI.COMM_WORLD
size = comm.Get_size()
rank = comm.Get_rank()
proc = MPI.Get_processor_name()

global_sum = comm.gather(rank)
if rank == 0:
    # Gathers into a list
    print(f"Sum: {sum(global_sum)}")

rank_array = np.array(rank, dtype=np.int32)
total_sum = np.zeros(1, dtype=np.int32)
comm.Allreduce(rank_array, total_sum, op=MPI.SUM)

if rank == 0:
    print(f"Sum: {sum(total_sum)}")
