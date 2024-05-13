import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

sns.set(style="whitegrid", font_scale=1.1)

# Read data from results.txt into a pandas DataFrame
data_strong = pd.read_csv(
    '/Users/carla/Documents/Courses/Spring2024/HPC_Lab/project05/hpc_python/ManagerWorker/results_strong.txt')

time_str = "timespent"
threads_str = "size"


clean = lambda x: x.replace('[', '').replace(']', '').split()[1:]
hist = data_strong[['TasksDoneByWorker']].values
hist = [np.array(clean(h[0]), dtype=np.int16) for h in hist]

df_strong = pd.DataFrame(data_strong[['ntasks', 'size']])
df_strong['hist'] = hist

# Strong Scaling Plot
ntasks = df_strong['ntasks'].unique()
threads = df_strong[threads_str].unique()

plt.figure(figsize=(10, 8))

for idx, n in enumerate(ntasks, start=1):
    plt.subplot(2, 1, idx)  # Creating subplots
    plt.title(f'{n} tasks')
    plt.xlabel('Worker ID')
    plt.ylabel('Tasks per worker')

    for i, thread in enumerate(threads):
        subset_ = df_strong[df_strong['ntasks'] == n]
        subset = subset_[subset_[threads_str] == thread]
        hist_mean = subset['hist'].values.squeeze().mean()
        offset = 0.8*i/len(threads)  # Calculate offset
        plt.bar(np.arange(1, len(hist_mean)+1) + offset, hist_mean,
                width=0.3, label=f'{thread} processes', alpha=0.8)

    plt.xticks(np.arange(1, len(hist_mean)+1),
               labels=np.arange(1, len(hist_mean)+1))
    plt.legend()

    # Minor ticks

plt.tight_layout()
plt.savefig('tasks_per_worker.pdf')

