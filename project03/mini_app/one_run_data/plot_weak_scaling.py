import pandas as pd
import matplotlib.pyplot as plt

# Read data from results.txt into a pandas DataFrame
data = pd.read_csv('results_weak.txt')
data_serial = pd.read_csv('results_serial.txt')

time_str = "timespent"
threads_str = "threads"
# Strong Scaling Plot

df = pd.DataFrame(data)
df_serial = pd.DataFrame(data_serial)

# Calculate weak scaling efficiency
time = df[time_str]
baseline_time = df_serial[time_str]
factor = [2**i for i in range(len(df[threads_str]))]
efficiency = baseline_time / time / factor

# Weak Scaling Plot
plt.figure(figsize=(10, 5))
plt.plot(df[threads_str], efficiency, marker='o')
plt.plot(factor, [1 for i in range(len(df[threads_str]))],
         linestyle='--', color='black', label='Ideal')

plt.title('Weak Scaling')
plt.xlabel('Number of Threads')
plt.ylabel('Speedup')
plt.legend()
plt.grid(True)
plt.xticks(df[threads_str])
plt.xscale('log', base=2)
plt.tight_layout()
plt.savefig('weak_scaling_plot.png')
