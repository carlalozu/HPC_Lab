import pandas as pd
import matplotlib.pyplot as plt

# Read data from results.txt into a pandas DataFrame
data_strong = pd.read_csv('results_strong.txt')
data_weak = pd.read_csv('results_weak.txt')

# Separate data by calculation type
calculation_types = ['Reduction', 'Critical']

# Strong Scaling Plot
plt.figure(figsize=(10, 5))
for calc_type in calculation_types:
    calc_data = data_strong[data_strong['Calculation'] == calc_type]
    t1 = calc_data[calc_data['Threads'] == 1]['Time'].values[0]
    plt.plot(calc_data['Threads'], t1/calc_data['Time'], marker='o', label=calc_type)
plt.plot(calc_data['Threads'], calc_data['Threads'], linestyle='--', color='green', label='Ideal')

plt.title('Strong Scaling')
plt.xlabel('Number of Threads')
plt.ylabel('Speedup')
plt.yscale('log', base=2)
plt.legend()
plt.grid(True)
plt.xticks(data_strong['Threads'])
plt.xscale('log', base=2)
plt.tight_layout()
plt.savefig('strong_scaling_plot.png')

# Weak Scaling Plot
plt.figure(figsize=(10, 5))
for calc_type in calculation_types:
    calc_data = data_weak[data_weak['Calculation'] == calc_type]
    t1 = calc_data[calc_data['Threads'] == 1]['Time'].values[0]
    plt.plot(calc_data['Threads'], t1/calc_data['Time'], marker='o', label=calc_type)
calc_data = data_weak[data_weak['Calculation'] == "Serial"]
plt.plot(calc_data['Threads'], calc_data['Speedup'], linestyle='--', color='green', label='Ideal')

plt.title('Weak Scaling')
plt.xlabel('Number of Threads')
plt.ylabel('Efficiency')
plt.legend()
plt.grid(True)
plt.xticks(data_weak['Threads'])
plt.xscale('log', base=2)
plt.tight_layout()
plt.savefig('weak_scaling_plot.png')
