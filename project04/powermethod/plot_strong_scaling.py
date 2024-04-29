import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

sns.set(style="whitegrid", font_scale=1.1)
# Read data from results.txt into a pandas DataFrame

files = ['results_strong_1', 'results_strong_div']
labels = ['One core', 'Multiple cores']
plt.figure(figsize=(10, 5))
for file, label in zip(files, labels):
    data_strong = pd.read_csv(f'{file}.txt')
    # data_serial = pd.read_csv('results_serial.txt')

    time_str = "time"
    threads_str = "threads"

    df_strong = pd.DataFrame(data_strong)
    # df_serial = pd.DataFrame(data_serial)

    df_complete = df_strong

    # Strong Scaling Plot
    nx = df_strong['size'].unique()
    threads = df_strong[threads_str].unique()

    # Plotting
    for n in nx:
        subset = df_complete[df_complete['size'] == n]
        # Calculate strong scaling efficiency
        median_time = subset.groupby(threads_str).mean().reset_index()

        baseline_time = median_time[median_time[threads_str] == 1][time_str].item()
        efficiency = baseline_time/median_time[time_str]
        plt.plot(threads, efficiency,
                marker='o', label=label)
plt.plot(threads, [2**i for i in range(len(threads))],
        linestyle='--', color='black', label='Ideal')

plt.xlabel('Number of Threads')
plt.ylabel('Speedup')
plt.yscale('log', base=2)
plt.legend()
plt.grid(True)
plt.xticks(threads)
plt.xscale('log', base=2)
plt.tight_layout()
plt.savefig('strong_scaling_plot.pdf')
