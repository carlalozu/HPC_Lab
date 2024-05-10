import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

sns.set(style="whitegrid", font_scale=1.1)
# Read data from results.txt into a pandas DataFrame
data_weak = pd.read_csv('/Users/carla/Documents/Courses/Spring2024/HPC_Lab/project05/mini_app_mpi/results_weak.txt')
data_serial = pd.read_csv('/Users/carla/Documents/Courses/Spring2024/HPC_Lab/project05/mini_app_mpi/results_serial.txt')

time_str = "timespent"
threads_str = "threads"

df_weak_ = pd.DataFrame(data_weak)
df_serial_ = pd.DataFrame(data_serial)

plt.figure(figsize=(10, 5))
for i, base_size in enumerate([64, 128, 256]):
    # Weak Scaling Plot

    df_weak = df_weak_[200*i:200*(i+1)]

    nx = df_weak['nx'].unique()
    threads = df_weak[threads_str].unique()

    df_serial = df_serial_[df_serial_['nx'].isin(nx)]

    # Calculate weak scaling efficiency
    time = df_weak[time_str]
    weak_median = df_weak.groupby('nx').median().reset_index()
    serial_median = df_serial.groupby('nx').median().reset_index()

    factor = [4**i for i in range(len(threads))]
    efficiency = serial_median[time_str] / weak_median[time_str] / factor

    # Weak Scaling Plot
    plt.plot(threads, efficiency, marker='o', label=f'Base Size: {base_size}')

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
