# M.L. for High Performance Computing Lab @USI & @ETHZ - malik.lechekhab@usi.ch 
"""
    inertial_part(A, coords)

Compute the bi-partions of graph `A` using inertial method based on the
`coords` of the graph.

# Examples
```julia-repl
julia> inertial_part(A, coords)
 1
 ⋮
 2
```
"""
function inertial_part(A, coords)

    d = size(coords)[2]
    best_cut = Inf
    p = ones(Int, size(coords)[1])
    p_temp = ones(Int, size(coords)[1])

    # 1. Compute the center of mass.
    center_of_mass = sum(coords, dims=1) / size(coords)[1]

    # 2. Construct the matrix M. (see pdf of the assignment)

    # 3. Compute the eigenvector associated with the smallest eigenvalue of M.

    # 4. Partition the nodes around line L 
    #    (use may use the function partition(coords, eigv))

    # 5. Return the indicator vector

    for dim in 1:d
        v = zeros(d)
        v[dim] = 1
        p1, p2 = partition(coords, v)
        p_temp[p1] .= 1
        p_temp[p2] .= 2
        this_cut = count_edge_cut(A, p_temp)
        if this_cut < best_cut
            best_cut = this_cut
            p = ones(Int, size(coords)[1])
            p[p1] .= 1
            p[p2] .= 2
        end
    end

    # RANDOM PARTITIONING - REMOVE AFTER COMPLETION OF THE EXERCISE
    n = size(A)[1];
    rng = MersenneTwister(1234);
    p = Int.(bitrand(rng, n));
    return p

end
