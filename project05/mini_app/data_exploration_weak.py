import seaborn as sns
import pandas as pd
import matplotlib.pyplot as plt

data_weak = pd.read_csv('results_weak.txt')
data_serial = pd.read_csv('results_serial.txt')

data_weak = pd.DataFrame(data_weak)
df_serial = pd.DataFrame(data_serial)

df_complete = pd.concat([data_weak, df_serial])

threads = data_weak['threads'].unique()


sns.set(style="whitegrid", font_scale=1.1)


# Create FacetGrid for each 'nx' value
g = sns.FacetGrid(df_complete, col="threads", col_wrap=3,
                  height=4, aspect=1, sharey=True, sharex=False)
g.map_dataframe(sns.boxplot, x="nx", y="timespent", palette="Set3")

g.set(yscale="log")

# Set titles and labels
g.set_axis_labels("Grid size", "Time (s)")

g.set_titles("Processes: {col_name}")

plt.tight_layout()

g.savefig("data_var_weak.pdf")
