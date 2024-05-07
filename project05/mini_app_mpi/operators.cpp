//******************************************
// operators.cpp
// based on min-app code written by Oliver Fuhrer, MeteoSwiss
// modified by Ben Cumming, CSCS
// *****************************************

// Description: Contains simple operators which can be used on 2/3d-meshes

#include "data.h"
#include "operators.h"
#include "stats.h"

namespace operators {

// compute the diffusion-reaction stencils
// s_old is the population concentration at time step k-1, s_new at k,
// and f is the residual (see Eq. (7) in Project 3).
void diffusion(data::Field const& s_old, data::Field const& s_new,
               data::Field& f) {
    using data::options;
    using data::domain;

    using data::bndE;
    using data::bndW;
    using data::bndN;
    using data::bndS;

    using data::buffE;
    using data::buffW;
    using data::buffN;
    using data::buffS;

    double alpha = options.alpha;
    double beta = options.beta;

    int nx = domain.nx;
    int ny = domain.ny;
    int iend  = nx - 1;
    int jend  = ny - 1;

    // Fill buffers
    for (int k = 0; k < nx; k++)
    {
        // These need to be inverted
        buffN[k] = s_new(k,jend);
        buffS[k] = s_new(k,0);
    }
    
    for (int k = 0; k < ny; k++)
    {
        buffE[k] = s_new(iend,k);
        buffW[k] = s_new(0,k);
    }

    // Exchange the ghost cells using non-blocking point-to-point
    // communication
    MPI_Request request[8];
    int tag1 = 1, tag2 = 2, tag3 = 3, tag4 = 4;
    int count = 0;
        
    // SEND
    //  to top
    MPI_Isend(&buffN[0], nx, MPI_DOUBLE, domain.neighbour_north, tag1, domain.comm_cart, &request[count++]);
    //  to bottom
    MPI_Isend(&buffS[0], nx, MPI_DOUBLE, domain.neighbour_south, tag2, domain.comm_cart, &request[count++]);
    //  to right
    MPI_Isend(&buffE[0], ny, MPI_DOUBLE, domain.neighbour_east, tag3, domain.comm_cart, &request[count++]);
    //  to left
    MPI_Isend(&buffW[0], ny, MPI_DOUBLE, domain.neighbour_west, tag4, domain.comm_cart, &request[count++]);

    // RECEIVE
    // from top
    MPI_Irecv(&bndN[0], nx, MPI_DOUBLE, domain.neighbour_north, tag2, domain.comm_cart, &request[count++]);
    // from bottom
    MPI_Irecv(&bndS[0], nx, MPI_DOUBLE, domain.neighbour_south, tag1, domain.comm_cart, &request[count++]);
    // from right
    MPI_Irecv(&bndE[0], ny, MPI_DOUBLE, domain.neighbour_east, tag4, domain.comm_cart, &request[count++]);
    // from left
    MPI_Irecv(&bndW[0], ny, MPI_DOUBLE, domain.neighbour_west, tag3, domain.comm_cart, &request[count++]);

    // the interior grid points
    for (int j=1; j < jend; j++) {
        for (int i=1; i < iend; i++) {
            f(i,j) = -(4. + alpha) * s_new(i,j)     // central point
                   + s_new(i-1,j) + s_new(i+1,j)    // east and west
                   + s_new(i,j-1) + s_new(i,j+1)    // north and south
                   + alpha * s_old(i,j)
                   + beta * s_new(i,j) * (1.0 - s_new(i,j));
        }
    }
    MPI_Status status[count];
    MPI_Waitall(count, request, status);

    // east boundary
    {
        int i = nx - 1;
        for (int j = 1; j < jend; j++) {
            f(i,j) = -(4. + alpha) * s_new(i,j)
                   + s_new(i-1,j) + bndE[j]
                   + s_new(i,j-1) + s_new(i,j+1)
                   + alpha * s_old(i,j)
                   + beta * s_new(i,j) * (1.0 - s_new(i,j));
        }
    }

    // west boundary
    {
        int i = 0;
        for (int j = 1; j < jend; j++) {
            f(i,j) = -(4. + alpha) * s_new(i,j)
                   + bndW[j]      + s_new(i+1,j)
                   + s_new(i,j-1) + s_new(i,j+1)
                   + alpha * s_old(i,j)
                   + beta * s_new(i,j) * (1.0 - s_new(i,j));
        }
    }

    // north boundary (plus NE and NW corners)
    {
        int j = ny - 1;

        {
            int i = 0; // NW corner
            f(i,j) = -(4. + alpha) * s_new(i,j)
                   + bndW[j]      + s_new(i+1,j)
                   + s_new(i,j-1) + bndN[i]
                   + alpha * s_old(i,j)
                   + beta * s_new(i,j) * (1.0 - s_new(i,j));
        }

        // north boundary
        for (int i = 1; i < iend; i++) {
            f(i,j) = -(4. + alpha) * s_new(i,j)
                   + s_new(i-1,j) + s_new(i+1,j)
                   + s_new(i,j-1) + bndN[i]
                   + alpha * s_old(i,j)
                   + beta * s_new(i,j) * (1.0 - s_new(i,j));
        }

        {
            int i = nx - 1; // NE corner
            f(i,j) = -(4. + alpha) * s_new(i,j)
                   + s_new(i-1,j) + bndE[j]
                   + s_new(i,j-1) + bndN[i]
                   + alpha * s_old(i,j)
                   + beta * s_new(i,j) * (1.0 - s_new(i,j));
        }
    }

    // south boundary (plus SW and SE corners)
    {
        int j = 0;
        {
            int i = 0; // SW corner
            f(i,j) = -(4. + alpha) * s_new(i,j)
                   + bndW[j] + s_new(i+1,j)
                   + bndS[i] + s_new(i,j+1)
                   + alpha * s_old(i,j)
                   + beta * s_new(i,j) * (1.0 - s_new(i,j));
        }

        // south boundary
        for (int i = 1; i < iend; i++) {
            f(i,j) = -(4. + alpha) * s_new(i,j)
                   + s_new(i-1,j) + s_new(i+1,j)
                   + bndS[i]      + s_new(i,j+1)
                   + alpha * s_old(i,j)
                   + beta * s_new(i,j) * (1.0 - s_new(i,j));
        }

        {
            int i = nx - 1; // SE corner
            f(i,j) = -(4. + alpha) * s_new(i,j)
                   + s_new(i-1,j) + bndE[j]
                   + bndS[i]      + s_new(i,j+1)
                   + alpha * s_old(i,j)
                   + beta * s_new(i,j) * (1.0 - s_new(i,j));
        }
    }

    // Accumulate the flop counts
    // 8 ops total per point
    stats::flops_diff += 12 * (nx - 2) * (ny - 2) // interior points
                      +  11 * (nx - 2  +  ny - 2) // NESW boundary points
                      +  11 * 4;                  // corner points
}

}
