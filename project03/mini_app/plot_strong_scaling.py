import pandas as pd
import matplotlib.pyplot as plt

# Read data from results.txt into a pandas DataFrame
data = pd.read_csv('results_omp.txt')
data_serial = pd.read_csv('results_serial.txt')

time_str = "timespent"
threads_str = "threads"
# Strong Scaling Plot

df = pd.DataFrame(data)
df_serial = pd.DataFrame(data_serial)

# Calculate strong scaling efficiency

plt.figure(figsize=(10, 5))
# Plotting
for i, nx in enumerate(df['nx'].unique()):
    subset = df[df['nx'] == nx]
    baseline_time = df_serial.iloc[i][time_str]
    efficiency = baseline_time / subset[time_str]
    plt.plot(subset[threads_str], efficiency,
             marker='o', label=f'nx={nx}')
plt.plot(subset[threads_str], [2**i for i in range(len(subset[threads_str]))],
         linestyle='--', color='black', label='Ideal')

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
