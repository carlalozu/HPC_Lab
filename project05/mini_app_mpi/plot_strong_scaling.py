import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

sns.set(style="whitegrid", font_scale=1.1)
# Read data from results.txt into a pandas DataFrame
data_strong = pd.read_csv('results_strong.txt')
data_serial = pd.read_csv('results_serial.txt')

time_str = "timespent"
threads_str = "threads"

df_strong = pd.DataFrame(data_strong)
df_serial = pd.DataFrame(data_serial)

df_complete = pd.concat([df_strong, df_serial])

# Strong Scaling Plot
nx = df_strong['nx'].unique()
threads = df_strong['threads'].unique()

plt.figure(figsize=(10, 5))
# Plotting
for n in nx:
    subset = df_complete[df_complete['nx'] == n]
    # Calculate strong scaling efficiency
    median_time = subset.groupby(threads_str).median().reset_index()
    baseline_time = median_time[median_time['threads'] == 0][time_str].item()
    efficiency = baseline_time/median_time[time_str][1:]
    plt.plot(threads, efficiency,
             marker='o', label=f'Grid size={n} x {n}')
plt.plot(threads, [2**i for i in range(len(threads))],
         linestyle='--', color='black', label='Ideal')

plt.title('Strong Scaling')
plt.xlabel('Number of Threads')
plt.ylabel('Speedup')
plt.yscale('log', base=2)
plt.legend()
plt.grid(True)
plt.xticks(threads)
plt.xscale('log', base=2)
plt.tight_layout()
plt.savefig('strong_scaling_plot.pdf')
