import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

sns.set(style="whitegrid", font_scale=1.1)
# Read data from results.txt into a pandas DataFrame
data_weak = pd.read_csv('results_weak.txt')
data_serial = pd.read_csv('results_serial.txt')

time_str = "timespent"
threads_str = "threads"

df_weak = pd.DataFrame(data_weak)
df_serial = pd.DataFrame(data_serial)

# Weak Scaling Plot
nx = df_weak['nx'].unique()
threads = df_weak[threads_str].unique()

# Calculate weak scaling efficiency
time = df_weak[time_str]
weak_median = df_weak.groupby('nx').median().reset_index()
serial_median = df_serial.groupby('nx').median().reset_index()

factor = [2**i for i in range(len(threads))]
efficiency = serial_median[time_str] / weak_median[time_str] / factor

# Weak Scaling Plot
plt.figure(figsize=(10, 5))
plt.plot(threads, efficiency, marker='o')
plt.plot(factor, [1 for i in range(len(threads))],
         linestyle='--', color='black', label='Ideal')

plt.title('Weak Scaling')
plt.xlabel('Number of Threads')
plt.ylabel('Speedup')
plt.legend()
plt.grid(True)
plt.xticks(threads)
plt.xscale('log', base=2)
plt.tight_layout()
plt.savefig('weak_scaling_plot.pdf')
