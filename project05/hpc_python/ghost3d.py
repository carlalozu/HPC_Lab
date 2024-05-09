from mpi4py import MPI

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
cartesian3d = comm.Create_cart(dims = [2,2,2],periods =[False,False,False],reorder=False)
coord3d = cartesian3d.Get_coords(rank)
print ("In 3D topology, Processor ",rank, " has coordinates ",coord3d)