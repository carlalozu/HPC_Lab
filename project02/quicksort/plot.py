import pandas as pd
import matplotlib.pyplot as plt

# Read data from results.txt into a pandas DataFrame
data = pd.read_csv('results_omp.txt')

time_str = "Time"
threads_str = "Threads"
# Strong Scaling Plot
# data.drop(data[data[threads_str] == 0].index, inplace=True)
array_size = sorted(list(set(data["Array Size"].values)))
plt.figure(figsize=(10, 5))
plt.plot(data[threads_str], data[threads_str], label='Ideal')
for size in array_size:
    data_size = data[data["Array Size"] == size]
    t1 = data_size[data_size[threads_str] == 1][time_str].values[0]
    plt.plot(data_size[threads_str], t1/data_size[time_str], marker='o', label=f"Array size: {size:.0e}")

plt.title('Strong Scaling')
plt.xlabel('Number of Threads')
plt.ylabel('Speedup')
plt.yscale('log', base=2)
plt.legend()
plt.grid(True)
plt.xticks(data[threads_str])
plt.xscale('log', base=2)
plt.tight_layout()
plt.savefig('strong_scaling_plot.png')
