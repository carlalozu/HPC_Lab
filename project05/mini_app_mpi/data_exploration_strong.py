import seaborn as sns
import pandas as pd
import matplotlib.pyplot as plt

data_strong = pd.read_csv('results_strong.txt')
data_serial = pd.read_csv('results_serial.txt')

df_strong = pd.DataFrame(data_strong)
df_serial = pd.DataFrame(data_serial)
nx = df_strong['nx'].unique()

df_serial = df_serial[df_serial['nx'].isin(nx)]
threads = df_strong['threads'].unique()

df_complete = pd.concat([df_strong, df_serial])



sns.set(style="whitegrid", font_scale=1.1)


# Create FacetGrid for each 'nx' value
g = sns.FacetGrid(df_complete, col="nx", col_wrap=3,
                  height=4, aspect=1, sharey=False)
g.map_dataframe(sns.boxplot, x="threads", y="timespent", palette="Set3")
g.set(yscale="log")
# g.add_legend(title="threads", loc="lower right")
# Set titles and labels
g.set_axis_labels("Processes", "Time (s)")

g.set_titles("{col_name} x {col_name}")

plt.tight_layout()

g.savefig("data_var_strong.pdf")
