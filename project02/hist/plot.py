import pandas as pd
import matplotlib.pyplot as plt

# Read data from results.txt into a pandas DataFrame
data = pd.read_csv('results.txt')

time_str = "Time"
threads_str = "Threads"
# Strong Scaling Plot
t1 = data[data[threads_str] == 0][time_str].values[0]
data.drop(data[data[threads_str] == 0].index, inplace=True)
plt.figure(figsize=(10, 5))
plt.plot(data[threads_str], t1/data[time_str], marker='o', label='Parallel')
plt.plot(data[threads_str], data[threads_str], label='Ideal')

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
