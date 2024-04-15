import seaborn as sns
import pandas as pd
import matplotlib.pyplot as plt

data_weak = pd.read_csv('results_weak.txt')
data_strong = pd.read_csv('results_strong.txt')
data_serial = pd.read_csv('results_serial.txt')

df_weak = pd.DataFrame(data_weak)
df_strong = pd.DataFrame(data_strong)
df_serial = pd.DataFrame(data_serial)

df_complete = pd.concat([df_strong, df_serial])

nx = df_strong['nx'].unique()
threads = df_strong['threads'].unique()


sns.set(style="whitegrid", font_scale=1.1)


# Create FacetGrid for each 'nx' value
g = sns.FacetGrid(df_complete, col="nx", col_wrap=3,
                  height=4, aspect=1, sharey=False)
g.map_dataframe(sns.boxplot, x="threads", y="timespent", palette="Set3")
g.set(yscale="log")
# g.add_legend(title="threads", loc="lower right")
# Set titles and labels
g.set_axis_labels("Threads", "Time (s)")

g.set_titles("nx = {col_name}")

plt.tight_layout()

g.savefig("data_var.pdf")
