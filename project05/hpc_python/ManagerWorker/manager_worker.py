import time
import sys
import numpy as np
from mpi4py import MPI  # MPI_Init and MPI_Finalize automatically called
import matplotlib.pyplot as plt
from mandelbrot_task import *
import matplotlib as mpl
mpl.use('Agg')

### mpirun python3 manager_worker.py 4001 4001 100 ###

# some parameters
MANAGER = 0       # rank of manager
TAG_TASK = 1  # task       message tag
TAG_TASK_DONE = 2  # tasks done message tag
TAG_DONE = 3  # done       message tag

VERBOSE = False


def manager(comm, tasks, TasksDoneByWorker):
    """
    The manager.

    Parameters
    ----------
    comm : mpi4py.MPI communicator
        MPI communicator
    tasks : list of objects with a do_task() method performing the task
        List of tasks to accomplish
    TasksDoneByWorker : dict
        Dictionary of number tasks done by worker

    Returns
    -------
    tasks_done_ : list of objects with _i_start, _nx_local, and _patch
    attributes
        List of tasks done
    """
    print("Initializing manager")
    ntasks = len(tasks)
    nworkers = comm.Get_size() - 1
    ntasks_sent = 0
    ntasks_done = 0
    task_index = 0
    tasks_done_ = []
    while ntasks_done < ntasks:
        # send new tasks
        while task_index < ntasks and ntasks_sent < nworkers:
            if VERBOSE:
                print("Sending tasks")
            comm.send(tasks[task_index], dest=ntasks_sent + 1, tag=TAG_TASK)
            ntasks_sent += 1
            task_index += 1

        # receive task done
        status = MPI.Status()
        task = comm.recv(source=MPI.ANY_SOURCE,
                         tag=TAG_TASK_DONE, status=status)
        tasks_done_.append(task)
        TasksDoneByWorker[status.source] += 1
        ntasks_done += 1
        ntasks_sent -= 1

        # send new task
        if task_index < ntasks:
            if VERBOSE:
                print(f"Sending worker {status.source} a new task")
            comm.send(tasks[task_index], dest=status.source, tag=TAG_TASK)
            ntasks_sent += 1
            task_index += 1
        # send done
        else:
            if VERBOSE:
                print(
                    f"All tasks done. Sending worker {status.source} empty task")
            comm.send(None, dest=status.source, tag=TAG_TASK)
    return tasks_done_


def worker(comm):
    """
    The worker.

    Parameters
    ----------
    comm : mpi4py.MPI communicator
        MPI communicator
    """
    while True:
        # receive task
        task = comm.recv(source=MANAGER, tag=TAG_TASK)
        if task is None:
            break
        if VERBOSE:
            print(f"Worker {comm.Get_rank()} has received task")
        # do task
        task.do_work()
        if VERBOSE:
            print(f"Worker {comm.Get_rank()} is done with task")
        # send task done
        comm.send(task, dest=MANAGER, tag=TAG_TASK_DONE)
    print(f"Worker {comm.Get_rank()} is done with all tasks")


def readcmdline(rank):
    """
    Read command line arguments

    Parameters
    ----------
    rank : int
        Rank of calling MPI process

    Returns
    -------
    nx : int
        number of gridpoints in x-direction
    ny : int
        number of gridpoints in y-direction
    ntasks : int
        number of tasks
    """
    # report usage
    if len(sys.argv) != 4:
        if rank == MANAGER:
            print("Usage: manager_worker nx ny ntasks")
            print("  nx     number of gridpoints in x-direction")
            print("  ny     number of gridpoints in y-direction")
            print("  ntasks number of tasks")
        sys.exit()

    # read nx, ny, ntasks
    nx = int(sys.argv[1])
    if nx < 1:
        sys.exit("nx must be a positive integer")
    ny = int(sys.argv[2])
    if ny < 1:
        sys.exit("ny must be a positive integer")
    ntasks = int(sys.argv[3])
    if ntasks < 1:
        sys.exit("ntasks must be a positive integer")

    return nx, ny, ntasks


if __name__ == "__main__":

    # get COMMON WORLD communicator, size & rank
    comm = MPI.COMM_WORLD
    size = comm.Get_size()
    my_rank = comm.Get_rank()
    TasksDoneByWorker = np.zeros(size, dtype=int)

    # report on MPI environment
    if my_rank == MANAGER:
        print(f"MPI initialized with {size:5d} processes")

    # read command line arguments
    nx, ny, ntasks = readcmdline(my_rank)

    # start timer
    timespent = - time.perf_counter()

    x_min = -2.
    x_max = +1.
    y_min = -1.5
    y_max = +1.5
    M = Mandelbrot(x_min, x_max, nx, y_min, y_max, ny, ntasks)
    tasks = M.get_tasks()

    if my_rank == MANAGER:
        tasks_done_ = manager(comm, tasks, TasksDoneByWorker)
        print("All tasks marked as done")
    else:
        worker(comm)

    comm.Barrier()
    if my_rank == MANAGER:
        im = M.combine_tasks(tasks_done_)
        plt.imshow(im.T, cmap="gray", extent=[x_min, x_max, y_min, y_max])
        plt.savefig("mandelbrot.png")

    # stop timer
    timespent += time.perf_counter()

    # inform that done
    if my_rank == MANAGER:
        print(f"Run took {timespent:f} seconds")
        for i in range(size):
            if i == MANAGER:
                continue
            print(f"Process {i:5d} has done {TasksDoneByWorker[i]:10d} tasks")
        print("Done.")
