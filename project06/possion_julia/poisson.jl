function poisson_analytic(x, y)
    lone_x_term = (1 - x .^ 2) / 2
    accum = 0
    for k in 1:2:100
        term_x = sin(k * pi * (1 + x) / 2) / (k^3 * sinh(k * pi))
        y_term = sinh(k * pi * (1 + y) / 2) + sinh(k * pi * (1 - y) / 2)
        accum += y_term * term_x
    end
    accum *= -16 / (pi^3)
    return accum + lone_x_term
end


function create_A(N, L)
    A = zeros(N^2, N^2)
    for n in 1:N^2
        # Main diagonal
        A[n, n] = -4

        # Boundary conditions: u_{i,j} = 0 if (i,j) on an edge.
        if n % N != 0 && n != N * N
            A[n, n+1] += 1
        end

        if (n - 1) % N != 0 && n != 1
            A[n, n-1] += 1
        end

        if n <= N * (N - 1)
            A[n, n+N] += 1
        end

        if n > N
            A[n, n-N] += 1
        end

    end
    h = L / (N + 1)

    return A ./ h^2
end


function create_A(N, L, x)
    A = zeros(N^2)
    for n in 1:N^2
        # Main diagonal
        A[n] += x[n] * -4

        # Boundary conditions: u_{i,j} = 0 if (i,j) on an edge.
        if n % N != 0 && n != N * N
            A[n] += x[n+1]
        end

        if (n - 1) % N != 0 && n != 1
            A[n] += x[n-1]
        end

        if n <= N * (N - 1)
            A[n] += x[n+N]
        end

        if n > N
            A[n] += x[n-N]
        end

    end
    h = L / (N + 1)

    return A ./ h^2
end