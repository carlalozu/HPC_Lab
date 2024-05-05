#include "data.h"

#include <iostream>
#include <cmath>



namespace data {

// fields that hold the solution
Field y_new;
Field y_old;

// fields that hold the boundary points
Field bndN;
Field bndE;
Field bndS;
Field bndW;

// buffers used during boundary halo communication
Field buffN;
Field buffE;
Field buffS;
Field buffW;

// global domain and local sub-domain
Discretization options;
SubDomain      domain;

void SubDomain::init(int mpi_rank, int mpi_size,
                     Discretization& discretization) {
    // determine the number of sub-domains in the x and y dimensions
    // using MPI_Dims_create
    int dims[2] = {0, 0};
    MPI_Dims_create(mpi_size, 2, dims);
    ndomy = dims[0];
    ndomx = dims[1];

    // create a 2D non-periodic Cartesian topology using MPI_Cart_create
    int periods[2] = {0, 0};
    int reorder = false;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, &comm_cart);

    // retrieve coordinates of the rank in the topology using MPI_Cart_coords
    int coords[2] = {0, 0};
    MPI_Cart_coords(comm_cart, mpi_rank, 2, coords);
    domy = coords[0] + 1;
    domx = coords[1] + 1;

    // TODO: set neighbours for all directions using MPI_Cart_shift
    enum DIRECTIONS
    {
        LEFT,
        RIGHT,
        UP,
        DOWN
    };
    int neighbours_ranks[4];

    // Let consider dims[0] = X, so the shift tells us our left and right neighbours
    MPI_Cart_shift(comm_cart, 1, 1, &neighbours_ranks[LEFT], &neighbours_ranks[RIGHT]);

    // Let consider dims[1] = Y, so the shift tells us our up and down neighbours
    MPI_Cart_shift(comm_cart, 0, 1, &neighbours_ranks[UP], &neighbours_ranks[DOWN]);

    // find your top/bottom/left/right neighbor using the communicator
    neighbour_west = neighbours_ranks[0];
    neighbour_east = neighbours_ranks[1];
    neighbour_north = neighbours_ranks[2];
    neighbour_south = neighbours_ranks[3];

    // get bounding box
    nx = discretization.nx / ndomx;
    ny = discretization.nx / ndomy;
    startx = (domx-1)*nx+1;
    starty = (domy-1)*ny+1;
    endx = startx + nx -1;
    endy = starty + ny -1;

    // get total number of grid points in this sub-domain
    N = nx*ny;

    rank = mpi_rank;
    size = mpi_size;
}

// print domain decomposition information to stdout
void SubDomain::print() {
    for (int irank = 0; irank < size; irank++) {
        if (irank == rank) {
            std::cout << "rank " << rank << " / " << size
                      << " : (" << domx << ", " << domy << ")"
                      << " neigh N:S " << neighbour_north
                      << ":"           << neighbour_south
                      << " neigh E:W " << neighbour_east
                      << ":"           << neighbour_west
                      << " local dims " << nx << " x " << ny
                      << " range (" << startx << " , " << starty <<")"
                      << std::endl;
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
}

}
