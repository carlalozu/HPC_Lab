import Pkg
Pkg.activate("Poisson")

using HDF5
using Plots

# Open the HDF5 file
h5file = h5open("solution.h5", "r")

key = keys(h5file)[1]
println("Keys: ", key)

# Read the vector data
u = read(h5file, key)

n = 10
u_ = reshape(u, n, n);

surface(u_, title="CG solution", xlabel="x", ylabel="y", zlabel="u(x,y)")


savefig(plot, "surface_plot.pdf")  # Saves the plot as a PDF file

# Close the HDF5 file
close(u)