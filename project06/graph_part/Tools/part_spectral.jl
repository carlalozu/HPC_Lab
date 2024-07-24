# M.L. for High Performance Computing Lab @USI & @ETHZ - malik.lechekhab@usi.ch 
"""
    spectral_part(A, fiedler=false)

Compute the bi-partions of graph `A` using spectral method.

If `fiedler` is true, return the entries of the fiedler vector.

# Examples
```julia-repl
julia> spectral_part(A)
 1
 ⋮
 2
```
"""
function spectral_part(A)
    n = size(A)[1]

    if n > 4*10^4
        @warn "graph is large. Computing eigen values may take too long."     
    end

    p = ones(Int, n)

    # 1. Construct the Laplacian matrix.
    d = sum(abs.(A).>0, dims=2)
    D = spdiagm(d[:,1])

    # L = D .- A
    L = D .- (abs.(A).>0).*1

    # 2. Compute its eigendecomposition.
    vals, vecs = eigs(L, nev=6, which=:SM)

    # 3. Label the vertices with the entries of the Fiedler vector.
    fiedler = vecs[:,2]

    # 4. Partition them around their median value, or 0.
    threshold = median(fiedler)

    # 5. Return the indicator vector
    indices = findall(x -> x > threshold, fiedler)
    p[indices] .= 2

    return p

end
