import numpy as np
import matplotlib.pyplot as plt

labels = ["Euler VII (Phase I)", "Euler VII (Phase 2)", "Euler III"]

# Roofline parameters for phase II
peak_performances = np.array([41.6, 39.2, 48])  # in GFlops/s
memory_bandwidths = np.array([27, 34, 12])  # in GB/s

# Operational intensities (FLOPs/Byte)
ops_intensity = peak_performances / memory_bandwidths
print(ops_intensity)

# Create the log-log roofline plot
plt.figure(figsize=(6, 4))

# Plot roofline
for i in range(3):
    x_roofline = np.linspace(0.0001,  2**9, 10000)

    y_roofline = np.minimum(
        x_roofline*memory_bandwidths[i], peak_performances[i])
    plt.loglog(x_roofline, y_roofline, label=labels[i])

# Add labels and legend
plt.xlabel('Operational Intensity (FLOPs/Byte)')
plt.ylabel('Performance (GFLOPs/s)')
plt.ylim([0.001, 100])
plt.xlim([2**(-7), 2**(9)])
plt.legend()

# Set y-axis to log base 2
plt.xscale('log', base=2)

# Show plot
plt.grid(True)
plt.tight_layout()
plt.savefig("task_2_4/roofline.pdf")
