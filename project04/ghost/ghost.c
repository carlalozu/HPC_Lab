/****************************************************************
 *                                                              *
 * This file has been written as a sample solution to an        *
 * exercise in a course given at the CSCS-USI Summer School     *
 * It is made freely available with the understanding that      *
 * every copy of this file must include this header and that    *
 * CSCS/USI take no responsibility for the use of the enclosed  *
 * teaching material.                                           *
 *                                                              *
 * Purpose: Exchange ghost cell in 2 directions using a topology*
 *                                                              *
 * Contents: C-Source                                           *
 *                                                              *
 ****************************************************************/

/* Use only 16 processes for this exercise
 * Send the ghost cell in two directions: left<->right and top<->bottom
 * ranks are connected in a cyclic manner, for instance, rank 0 and 12 are connected
 *
 * process decomposition on 4*4 grid
 *
 * |-----------|
 * | 0| 1| 2| 3|
 * |-----------|
 * | 4| 5| 6| 7|
 * |-----------|
 * | 8| 9|10|11|
 * |-----------|
 * |12|13|14|15|
 * |-----------|
 *
 * Each process works on a 6*6 (SUBDOMAIN) block of data
 * the D corresponds to data, g corresponds to "ghost cells"
 * xggggggggggx
 * gDDDDDDDDDDg
 * gDDDDDDDDDDg
 * gDDDDDDDDDDg
 * gDDDDDDDDDDg
 * gDDDDDDDDDDg
 * gDDDDDDDDDDg
 * gDDDDDDDDDDg
 * gDDDDDDDDDDg
 * gDDDDDDDDDDg
 * gDDDDDDDDDDg
 * xggggggggggx
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <stdbool.h>
#include <string.h>

#define SUBDOMAIN 6
#define DOMAINSIZE (SUBDOMAIN + 2)

int main(int argc, char *argv[])
{
    int rank, size, i, j, dims[2], periods[2], rank_top, rank_bottom, rank_left, rank_right, tag = 1;
    double data[DOMAINSIZE * DOMAINSIZE];
    MPI_Request request;
    MPI_Status status;
    MPI_Comm comm_cart;
    MPI_Datatype data_ghost_row;
    MPI_Datatype data_ghost_col;

    // Initialize MPI
    int ierr;
    ierr = MPI_Init(&argc, &argv);
    if (ierr != MPI_SUCCESS) {
        fprintf(stderr, "Error initializing MPI.\n");
        MPI_Abort(MPI_COMM_WORLD, ierr);
    }


    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 16)
    {
        printf("please run this with 16 processors\n");
        MPI_Finalize();
        exit(1);
    }

    // initialize the domain
    for (i = 0; i < DOMAINSIZE * DOMAINSIZE; i++)
    {
        data[i] = rank;
    }

    // set the dimensions of the processor grid and periodic boundaries in both dimensions
    dims[0] = 0;
    dims[1] = 0;
    periods[0] = true;
    periods[1] = true;

    // Create a Cartesian communicator (4*4) with periodic boundaries (we do not allow
    // the reordering of ranks) and use it to find your neighboring
    // ranks in all dimensions in a cyclic manner.
    int reorder = false;

    MPI_Dims_create(size, 2, dims);
    MPI_Comm new_communicator;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, &new_communicator);

    enum DIRECTIONS
    {
        LEFT,
        RIGHT,
        UP,
        DOWN
    };
    char *neighbours_names[4] = {"left", "right", "up", "down"};
    int neighbours_ranks[4];

    // Let consider dims[0] = X, so the shift tells us our left and right neighbours
    MPI_Cart_shift(new_communicator, 1, 1, &neighbours_ranks[LEFT], &neighbours_ranks[RIGHT]);

    // Let consider dims[1] = Y, so the shift tells us our up and down neighbours
    MPI_Cart_shift(new_communicator, 0, 1, &neighbours_ranks[UP], &neighbours_ranks[DOWN]);

    // find your top/bottom/left/right neighbor using the new communicator, see MPI_Cart_shift()
    rank_left = neighbours_ranks[0];
    rank_right = neighbours_ranks[1];
    rank_top = neighbours_ranks[2];
    rank_bottom = neighbours_ranks[3];

    // create derived datatype data_ghost_row
    MPI_Type_contiguous(SUBDOMAIN, MPI_DOUBLE, &data_ghost_row);
    MPI_Type_commit(&data_ghost_row);

    // create a datatype for sending the column, 
    // see MPI_Type_vector() and MPI_Type_commit()
    MPI_Type_vector(SUBDOMAIN, 1, DOMAINSIZE, MPI_DOUBLE, &data_ghost_col);
    MPI_Type_commit(&data_ghost_col);

    //  ghost cell exchange with the neighbouring cells in all directions
    //  use MPI_Recv(), MPI_Send(), MPI_Wait() or other viable alternatives

    // send and receive vectors not just values
    double a[SUBDOMAIN];
    double b[SUBDOMAIN];

    for (i = 0; i <= SUBDOMAIN; i++)
    {
        // Make vector of rank number
        a[i] = rank;
    }

    //  to the top
    MPI_Send(&data[1], 1, data_ghost_row, rank_top, tag, MPI_COMM_WORLD);
    //  to the bottom
    MPI_Send(&data[DOMAINSIZE*(DOMAINSIZE-1)+1], 1, data_ghost_row, rank_bottom, tag, MPI_COMM_WORLD);
    //  to the left
    MPI_Send(&data[DOMAINSIZE], 1, data_ghost_col, rank_left, tag, MPI_COMM_WORLD);
    //  to the right
    MPI_Send(&data[DOMAINSIZE*2-1], 1, data_ghost_col, rank_right, tag, MPI_COMM_WORLD);

    // from bottom
    MPI_Recv(&data[DOMAINSIZE*(DOMAINSIZE-1)+1], 1, data_ghost_row, rank_bottom, tag, MPI_COMM_WORLD, &status);
    // from top
    MPI_Recv(&data[1], 1, data_ghost_row, rank_top, tag, MPI_COMM_WORLD, &status);
    // from right
    MPI_Recv(&data[DOMAINSIZE*2-1], 1, data_ghost_col, rank_right, tag, MPI_COMM_WORLD, &status);
    // from left
    MPI_Recv(&data[DOMAINSIZE], 1, data_ghost_col, rank_left, tag, MPI_COMM_WORLD, &status);

    if (rank == 9)
    {
        printf("data of rank 9 after communication\n");
        for (j = 0; j < DOMAINSIZE; j++)
        {
            for (i = 0; i < DOMAINSIZE; i++)
            {
                printf("%.1f ", data[i + j * DOMAINSIZE]);
            }
            printf("\n");
        }
    }

    // Free MPI resources (e.g., types and communicators)
    MPI_Type_free(&data_ghost_row);
    MPI_Type_free(&data_ghost_col);
    MPI_Comm_free(&new_communicator);

    // Finalize MPI
    ierr = MPI_Finalize();
    if (ierr != MPI_SUCCESS) {
        fprintf(stderr, "Error finalizing MPI.\n");
        exit(1);
    }

    return 0;
}
