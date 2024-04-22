#include <mpi.h> // MPI
#include <stdio.h>

int main(int argc, char *argv[])
{

  // Initialize MPI, get size and rank
  int size, rank;
  char pname[MPI_MAX_PROCESSOR_NAME];
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Status status;

  // IMPLEMENT: Ring sum algorithm
  int rank_dest, rank_source;
  int out_msg, in_msg;

  int sum = 0, i; // initialize sum
  for (i = 0; i < size; i++)
  {

    rank_source = rank - 1;
    if (rank == 0)
      rank_source = size - 1;

    rank_dest = rank + 1;
    if (rank == size - 1)
      rank_dest = 0;

    if (i == 0)
    {
      out_msg = rank;
    }

    MPI_Send(&out_msg, 1, MPI_INT, rank_dest, 0, MPI_COMM_WORLD);
    MPI_Recv(&in_msg, 1, MPI_INT, rank_source, 0, MPI_COMM_WORLD, &status);

    sum = sum + in_msg;
    out_msg = in_msg;
  }

  printf("Process %i: Sum = %i\n", rank, sum);

  // Finalize MPI
  MPI_Finalize();

  return 0;
}
