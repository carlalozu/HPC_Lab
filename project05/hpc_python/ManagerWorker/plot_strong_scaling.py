import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

sns.set(style="whitegrid", font_scale=1.1)
# Read data from results.txt into a pandas DataFrame
data_strong = pd.read_csv('results_strong.txt')

time_str = "timespent"
threads_str = "size"

df_strong = pd.DataFrame(data_strong[['ntasks',threads_str,time_str]])

# Strong Scaling Plot
ntasks = df_strong['ntasks'].unique()
threads = df_strong[threads_str].unique()


plt.figure(figsize=(10, 5))
# Plotting
for n in ntasks:
    subset = df_strong[df_strong['ntasks'] == n]
    # Calculate strong scaling efficiency
    median_time = subset.groupby(threads_str).median().reset_index()
    baseline_time = median_time[median_time[threads_str] == 2][time_str].item()
    efficiency = baseline_time/median_time[time_str]
    plt.plot(threads, efficiency,
             marker='o', label=f'Number of tasks={n}')
plt.plot(threads, [2**i for i in range(len(threads))],
         linestyle='--', color='black', label='Ideal')

plt.title('Strong Scaling')
plt.xlabel('Number of Processes')
plt.ylabel('Speedup')
plt.yscale('log', base=2)
plt.legend()
plt.grid(True)
plt.xticks(threads)
plt.xscale('log', base=2)
plt.tight_layout()
plt.savefig('strong_scaling_plot.pdf')
