from mpi4py import MPI
import numpy as np

# This is to create default communicator and get the rank
comm = MPI.COMM_WORLD
rank = comm.Get_rank()
size = comm.Get_size()
if (rank==0):
    print("In periodic 2D topology \n")

# Create dims depending on number of ranks
dims = [0,0]
dims = MPI.Compute_dims(size, dims)

# Create cartesian grid
periods = [True, True]
cart2d = comm.Create_cart(dims, periods,reorder=False)
coords = cart2d.Get_coords(rank)

# Get coordinates of the neighbours
west, east = cart2d.Shift(direction = 1, disp=1)
north, south = cart2d.Shift(direction = 0, disp=1)


print(f"Processor {rank}:")
print(f"    has coordinates {coords}")
print(f"    has neighbour west {west} and east {east}")
print(f"    has neighbour north {north} and south {south} \n")

# Exchange ranks
my_neighbours = [west, east, north, south]
myranks_send = np.array([rank for i in range(4)], dtype=np.int32)
myranks_recv = np.empty(4, dtype=np.int32)
recv_rank = np.empty(1, dtype=np.int32)
tag = 1

# Send data to neighbors and receive data from other neighbors
for i, j in zip(range(4), [1,0,3,2]):
    comm.Sendrecv(myranks_send[i], my_neighbours[i], tag, recv_rank, my_neighbours[j])
    myranks_recv[j] = recv_rank[0]

# Ensure all processes complete communication before printing
comm.Barrier()
print("Processor", rank, "exchanged ranks (W,E,N,S):", myranks_recv)
