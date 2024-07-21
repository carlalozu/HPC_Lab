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
    p = ones(Int, size(coords)[1])
    mass_ = zeros(Float64, size(coords)[1])

    # 1. Compute the center of mass.
    for dim in 1:d
        mass_[dim] = sum(coords[:,dim])/size(coords)[1]
    end

    # 2. Construct the matrix M. (see pdf of the assignment)
    #   (assumes 2 dim)
    Sxx = sum(coords[:,1].-mass_[1]).^2
    Syy = sum(coords[:,2].-mass_[2]).^2
    Sxy = sum(coords[:,1].-mass_[1]).*sum(coords[:,2].-mass_[2])

    M = [Syy, Sxy, Sxy, Sxx]
    M = reshape(M, d, d)

    # 3. Compute the eigenvector associated with the smallest eigenvalue of M.
    vals, vecs = eigen(M)

    # 4. Partition the nodes around line L 
    #    (use may use the function partition(coords, eigv))
    p1, p2 = partition(coords, vecs[:,1])

    # 5. Return the indicator vector
    p[p1] .= 1
    p[p2] .= 2

    return p

end
