# M.L. for High Performance Computing Lab @USI & @ETHZ - malik.lechekhab@usi.ch 
"""
    rec_bisection(method, levels, A, coords=zeros(0), vn=zeros(0))

Compute recursive partitioning of graph `A` using a specified `method` and
number of `levels`.

If the `method` is `coords`-based, coordinates must be passed.

# Examples
```julia-repl
julia> rec_bisection("spectralPart", 3, A)
 5
 ⋮
 2

julia> rec_bisection("coordinatePart", 3, A, coords)
 1
 ⋮
 8
```
"""
function rec_bisection(method, levels, A, coords=zeros(0,0), vn=zeros(0))

    n = size(A, 1)
    l = 2^levels
    index = 1:n
    p = ones(Int, n)

    function recursion(A_, levels_, index_, count_)
        if levels_ % 2 == 0
            levels_ = div(levels_, 2)

            if method == "spectralPart"
                p_ = spectral_part(A_)
            elseif method == "inertialPart"
                p_ = inertial_part(A_, coords[index_, :])
            elseif method == "coordinatePart"
                p_ = coordinate_part(A_, coords[index_, :])
            else 
                println("Method not available")
                return
            end

            a1 = p_ .== 1
            a2 = p_ .== 2

            A1_ = A_[a1, a1]
            A2_ = A_[a2, a2]

            recursion(A1_, levels_, index_[a1], count_)
            recursion(A2_, levels_, index_[a2], count_ + levels_)
        else
            # println(index_, count_)
            p[[i in index_ for i in 1:n]] .= count_
        end
    end

    recursion(A, l, index, 1)
    return p
end