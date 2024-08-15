from mpi4py import MPI
import numpy as np

# get comm, size, rank & host name
comm = MPI.COMM_WORLD
size = comm.Get_size()
rank = comm.Get_rank()
proc = MPI.Get_processor_name()

# using lowercase functions
global_sum = comm.allreduce(rank, op=MPI.SUM)
if rank == 0:
    print(f"Sum: {global_sum}")

# using uppercase functions
rank_array = np.array(rank, dtype=np.int32)
total_sum = np.zeros(1, dtype=np.int32)
comm.Allreduce(rank_array, total_sum, op=MPI.SUM)
if rank == 0:
    print(f"Sum: {total_sum[0]}")
