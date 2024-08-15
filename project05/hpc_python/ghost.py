from mpi4py import MPI
import numpy as np
from copy import deepcopy
np.set_printoptions(precision=1)

SUBDOMAIN = 6
DOMAINSIZE = SUBDOMAIN + 2

# This is to create default communicator and get the rank
comm = MPI.COMM_WORLD
rank = comm.Get_rank()
size = comm.Get_size()
if (rank == 0):
    print("In periodic 2D topology \n")

# Create dims depending on number of ranks
dims = [0, 0]
dims = MPI.Compute_dims(size, dims)

# Create cartesian grid
periods = [True, True]
cart2d = comm.Create_cart(dims, periods, reorder=False)
coords = cart2d.Get_coords(rank)

# Get coordinates of the neighbours
west, east = cart2d.Shift(direction=1, disp=1)
north, south = cart2d.Shift(direction=0, disp=1)


print(f"Processor {rank}:")
print(f"    has coordinates {coords}")
print(f"    has neighbour west {west} and east {east}")
print(f"    has neighbour north {north} and south {south} \n")

data = np.empty((DOMAINSIZE, DOMAINSIZE), dtype=np.int32)
data.fill(rank)
new_data = data.copy()

# Contiguous arrays to store columns
col_east = np.empty(SUBDOMAIN, dtype=np.int32)
col_west = np.empty(SUBDOMAIN, dtype=np.int32)

# Send and receive
comm.Sendrecv(data[1, 1:-1],            north, 1, new_data[DOMAINSIZE-1, 1:-1], south, 1)
comm.Sendrecv(data[DOMAINSIZE-2, 1:-1], south, 2, new_data[0, 1:-1], north, 2)
comm.Sendrecv(np.array(data[1:-1, DOMAINSIZE-2].data, dtype=np.int32),
                                        east, 4, col_west, west, 4)
comm.Sendrecv(np.array(data[1:-1, 1].data, dtype=np.int32),
                                        west, 3, col_east, east, 3)

new_data[1:-1, DOMAINSIZE-1] = col_east.data
new_data[1:-1, 0] = col_west.data

# Wait till done
comm.Barrier()

print(new_data, "\n")
