import os
import pandas as pd
from numpy import median, nan
from scipy.stats import ranksums
import matplotlib.pyplot as plt
import seaborn as sns

from utils import sort_frame, add_space, add_space_list, remove_prefixe


############# Read file #############

# Read a line of a stat file and write corresponding data frame when encountering algo for the first time
def read_line(frame, algo_frame, line, algo):
    index = frame.shape[0]
    index_algo = algo_frame.shape[0]
    frame.at[index, 'algo'] = add_space(algo)
    algo_frame.at[index_algo, 'algo'] = add_space(algo)
    for stat in line.rstrip().split(' '): # Separate stat on space
        values = stat.rstrip().split(':') # Separate name and value on :
        if len(values) == 1: # If gen number
            frame.at[index, 'generations'] = float(values[0])
            algo_frame.at[index_algo, 'generations'] = float(values[0])
        elif values[0] == "Eval": # If eval number
            frame.at[index, 'evaluations'] = float(values[1])
            algo_frame.at[index_algo, 'evaluations'] = float(values[1])
        else: # If any other stats
            if not(values[0] in frame.columns): # Not encountered before
                frame[values[0]] = [ 0.0  for i in range(index) ] + [float(values[1])]
            else: # Already known
                frame.at[index, values[0]] = float(values[1])
    return frame, algo_frame

# Read a line of a stat file and write corresponding data frame when already encountered the algo
def read_line_algo(frame, line, algo, gen_number, eval_number):
    index = frame.shape[0]
    frame.at[index, 'algo'] = algo
    frame.at[index, 'generations'] = gen_number
    frame.at[index, 'evaluations'] = eval_number
    for stat in line.rstrip().split(' '): # Separate stat on space
        values = stat.rstrip().split(':') # Separate name and value on :
        if len(values) > 1 and values[0] != "Eval": # If not the gen or eval number
            if not(values[0] in frame.columns): # Not encountered before
                frame[values[0]] = [ 0.0  for i in range(index) ] + [float(values[1])]
            else: # Already known
                frame.at[index, values[0]] = float(values[1])
    return frame


# Open a list of files anf fill the coresponding frame
def read_files(path_frame):
    frame = pd.DataFrame(columns = ["generations", "evaluations", "algo"])
    algo_frame = pd.DataFrame(columns = ["generations", "evaluations", "algo"])
    # For each file
    for i in path_frame.index:
        algo = add_space(path_frame.at[i, 'algo'])
        # If already encounter this algo, use same values for eval and gen
        if algo in algo_frame['algo'].values: 
            algo_partial = algo_frame[algo_frame['algo'] == algo]
            if os.path.isfile(path_frame.at[i, 'path']):
                with open(path_frame.at[i, 'path']) as f:
                    n_line = 0
                    for line in f: # For each line
                        if n_line < algo_partial.shape[0]:
                            frame = read_line_algo(frame, line, algo, \
                                                    algo_partial['generations'].iloc[n_line], \
                                                    algo_partial['evaluations'].iloc[n_line])
                        else:
                            frame, algo_frame = read_line(frame, algo_frame, line, algo)
                        n_line += 1
        # Else fill in common gen and eval values
        else:
            if os.path.isfile(path_frame.at[i, 'path']):
                with open(path_frame.at[i, 'path']) as f:
                    for line in f: # For each line
                        frame, algo_frame = read_line(frame, algo_frame, line, algo)
    frame = frame.fillna(0) # Replace all NaN with 0
    return sort_frame(frame) # Sort algo by alphabetic order


################### p-value ##############

# Return the maximum number of a column for a given algo
def max_number(frame, column, algo):
    return frame[frame["algo"] == algo][column].max()

# Compute one p-value
def p_value_ranksum(frame, reference_variant, compare_variant, graph):
    _, p = ranksums( frame[(frame["algo"] == reference_variant) & \
                           (frame["evaluations"] == max_number(frame, "evaluations", reference_variant)) \
                          ][graph].to_numpy(),
                     frame[(frame["algo"] == compare_variant) & \
                           (frame["evaluations"] == max_number(frame, "evaluations", compare_variant)) \
                          ][graph].to_numpy()
                   )
    return p

# Compute all p-values between all algos and save them in file
def compute_p_values(frame, task, algo_list, graph, path):
    p_frame = pd.DataFrame(columns=['Reference variant','Variant','p-value'])
    for reference_variant in algo_list:
        for compare_variant in algo_list:
            p_frame = p_frame.append({'Reference variant' : reference_variant,
                                      'Variant' : compare_variant,
                                      'p-value' : p_value_ranksum(frame, reference_variant, compare_variant, graph)
                                     }, ignore_index=True)
    p_frame = p_frame.pivot(index='Reference variant', columns='Variant', values='p-value')
    p_file = open(os.path.join(path, task + "_" + "pvalue_" + graph + ".md"), "a")
    

############# Plot #############

# Generate one graph
def plot_graph(frame, x, y, title, picname, rescale_x = True, legend = True):
    plt.figure(figsize=(8, 5))
    sns_plot = sns.lineplot(x=x, y=y, hue='algo', data=frame, dashes=False, estimator=median)
    plt.setp(sns_plot.lines,linewidth=2)
    max_x = frame.groupby('algo')[x].max().min() if rescale_x else frame.groupby('algo')[x].max().max()
    plt.xlim(0, 1.05 * max_x)
    sns_plot.set_xlabel('Number of ' + add_space(remove_prefixe(x)))
    sns_plot.set_ylabel(add_space(remove_prefixe(y)))
    sns_plot.set_title(add_space(title))
    handles, labels = sns_plot.get_legend_handles_labels()
    if legend:
        sns_plot.legend(handles=handles[1:], labels=labels[1:])
    else:
        sns_plot.legend_.remove()
    plt.savefig(picname)
    plt.close()

# Generate all graphs with a given x-axis column
def plot_graphs(frame, info_frame, column, task, path, rescale_x = True, p_value = False, legend = True):
    sns.set(style="whitegrid")
    sns.set_palette(info_frame['colors'].values.tolist())
    end_path = "_" + task + "_" + column + ".png" # "_" + suffixe + ".png"
    for item in frame.columns.values:
        if (item != 'generations') and (item != 'evaluations') and (item != 'algo'):
            plot_graph(frame[['generations', 'evaluations', 'algo', item]], column, item, task, 
                       os.path.join(path, item + end_path), rescale_x, legend) 
            if p_value:
                compute_p_values(frame[['generations', 'evaluations', 'algo', item]], task, 
                                 add_space_list(info_frame['algo']).values, item, path)


########## Main function #############

# Plot the gen and eval graph from a task name and a path to stats
def graph(path, task, stat_frame, path_frame, p_value = False, legend = True):
    frame = read_files(path_frame) # Read data
    info_frame = sort_frame(stat_frame)
    plot_graphs(frame, info_frame, 'generations', task, path, False, False, legend) # Gen plot
    plot_graphs(frame, info_frame, 'evaluations', task, path, True, p_value, legend) # Eval plot

