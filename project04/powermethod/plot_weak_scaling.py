import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
import math
sns.set(style="whitegrid", font_scale=1.1)
# Read data from results.txt into a pandas DataFrame

files = ['results_weak_1', 'results_weak_div']
labels = ['One core', 'Multiple cores']

time_str = "time"
threads_str = "threads"

plt.figure(figsize=(10, 5))
for file, label in zip(files, labels):
    data_weak = pd.read_csv(f'{file}.txt')
    df_weak = pd.DataFrame(data_weak)
    # df_serial = pd.DataFrame(data_serial)

    # Weak Scaling Plot
    nx = df_weak['size'].unique()
    threads = df_weak[threads_str].unique()

    # Calculate weak scaling efficiency
    time = df_weak[time_str]
    weak_median = df_weak.groupby('threads').mean().reset_index()
    weak_std = df_weak.groupby('threads').std().reset_index()

    weak_median['std'] = weak_std['time']
    serial_median = df_weak[df_weak[threads_str]==1].mean()

    factor =  weak_median['size'] / np.ceil(np.sqrt(weak_median['threads'])) / weak_median['size']
    efficiency = serial_median['time'] / weak_median[time_str] * factor

    # Weak Scaling Plot
    plt.plot(threads, efficiency, marker='o', label = label)
plt.plot(threads, [1 for i in range(len(threads))],
         linestyle='--', color='black', label='Ideal')

plt.xlabel('Number of Threads')
plt.ylabel('Speedup')
plt.legend()
plt.grid(True)
plt.xticks(threads)
plt.xscale('log', base=2)
plt.tight_layout()
plt.savefig('weak_scaling_plot.pdf')
