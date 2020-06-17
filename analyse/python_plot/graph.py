########## Import #########
import os
import sys
import math
import matplotlib.pyplot as plt
from matplotlib.colors import ListedColormap
import seaborn as sns
import pandas as pd
from numpy import median, floor, flatnonzero
from scipy.stats import ranksums
import glob

############## Global list ###########

prefixes = ["cell_fitness_", "cell_stability_", "best_fitness_", "best_stability_", "filling_"]

##### Name appearance function #######

# Remove the prefixe from a name
def remove_prefixe(name):
    for prefixe in prefixes:
        if prefixe in name:
            name = name[len(prefixe):]
    return name

# Change between _ and space
def add_space(name):
   return name.replace("_", " ")
def remove_space(name):
   return name.replace(" ", "_")

############# Pre-prosesing #################

#  Sort a frame in alphabetic order
def sort_frame(frame):
    sort_frame = pd.concat([frame[frame["variant"].str.contains('Baseline')], \
                        frame[~frame["variant"].str.contains('Baseline')].sort_values(by="variant")])
    return sort_frame

# round all eval number to number before
def set_same_x(frame, variant_list, colun_name, reproductions):
    for variant_name in variant_list:
        if variant_name in frame.values:
            # Read all eval values for the given variant
            values_all = frame[frame["variant"] == variant_name][colun_name].sort_values()
            values_select = []
            # Find the values that should be kept for each step
            for i in range (0, values_all.shape[0], \
                            int(reproductions[reproductions["variant"] == variant_name]["number"].values)):
                values_select.append(values_all.iloc[i])
            # Replace all others with these values
            for i in range (len(values_select)-1):
                colum_line = flatnonzero((frame["variant"] == variant_name) \
                    & (frame[colun_name] >= values_select[i]) & (frame[colun_name] < values_select[i+1])).tolist()
                for index_number in colum_line:
                    frame.iloc[index_number,0] = values_select[i]
    return frame



########### Plot function #########

# Plot a line graph
def plot_graph_from_csv(frame, x, xname, y, plot_legend, max_x, task, picname):
    plt.figure(figsize=(8, 5))
    sns_plot = sns.lineplot(x=x, y=y, hue="variant", data=frame, 
                            dashes=False, estimator=median)
    plt.setp(sns_plot.lines,linewidth=2)
    plt.xlim(0, 1.05*max_x)
    sns_plot.set_xlabel(add_space(remove_prefixe(xname)))
    sns_plot.set_ylabel(add_space(remove_prefixe(y)))
    sns_plot.set_title(add_space(task))
    handles, labels = sns_plot.get_legend_handles_labels()
    if plot_legend:
      sns_plot.legend(handles=handles[1:], labels=labels[1:])
    else:
      sns_plot.legend_.remove()
    plt.savefig(picname)
    plt.close()


################# p value ##################

def p_value_ranksum(frame, reference_variant, compare_variant, graph):
    max_eval_ref = frame[frame["variant"]==reference_variant]["eval"].max()
    max_eval_comp = frame[frame["variant"]==compare_variant]["eval"].max()
    _, p = ranksums(
            frame[(frame["variant"]==reference_variant)  
                & (frame["eval"]==max_eval_ref)][graph].to_numpy(),
            frame[(frame["variant"]==compare_variant)  
                & (frame["eval"]==max_eval_comp)][graph].to_numpy())
    return p

def compute_p_values(frame, task, variant_list, graph, path):
    p_frame = pd.DataFrame(columns=['Reference variant','Variant','p-value'])
    for reference_variant in variant_list:
        for compare_variant in variant_list:
            p = p_value_ranksum(frame, reference_variant, compare_variant, graph)
            p_frame = p_frame.append({'Reference variant':reference_variant,
                                      'Variant':compare_variant,
                                      'p-value':p}, ignore_index=True)
    p_frame=p_frame.pivot(index='Reference variant', columns='Variant', values='p-value')
    p_file = open(path+"/" + task + "_" + "pvalue_" + graph + ".md", "a")
    p_file.write(p_frame.to_markdown())
    p_file.close()



################# Main ####################

# Read inputs
path = sys.argv[1] + "/"
size_path = len(path)
compute_p_value = False
plot_one_legend_only = False
if (len(sys.argv) > 2) and ('p_value' in sys.argv[2]):
    compute_p_value = True
    if (len(sys.argv) > 3) and ('legend' in sys.argv[3]):
        plot_one_legend_only = True
elif (len(sys.argv) > 2) and ('legend' in sys.argv[2]):
    plot_one_legend_only = True


# Read the task file
print("\n \nPrinting graphs and distributions \n")
sns.set(style="whitegrid", font_scale = 1.5)
tasks = pd.read_csv(path + "tasks_overview.dat", delimiter = ' ', header = None, names=["task", "stat_file"])


# Go through all tasks a first time to read all variants runs informations
variants = pd.DataFrame(columns=['task','variant','number'])
for task_number in range (tasks.shape[0]):

    # Read the corresponding stat file
    reproductions = sort_frame( pd.read_csv(path + tasks["stat_file"][task_number], delimiter = ' ',  \
            header = None, names=["number", "variant"]) [1:])

    # For each variant fill in the panda frame
    for i in range (0, reproductions.shape[0]):
        reproductions.iloc[i,1] = add_space(reproductions.iloc[i,1])
        variants = variants.append({'task':tasks["task"][task_number],\
				    'variant':reproductions["variant"].iloc[i],\
				    'number':reproductions["number"].iloc[i]},\
				    ignore_index=True)


# Create color palette using the toal number of variants
var_lists = sort_frame(variants)["variant"].drop_duplicates().values
palette_list = pd.DataFrame(data = {'variant': var_lists,
				    'colors' : sns.husl_palette(len(var_lists), 
                                                                l=0.7)})


# Go through all tasks a second time to read and plot datas
for task_number in range (tasks.shape[0]):

    task = tasks["task"][task_number] # Task name
    print("Printing all graphs for tasks:", task)
    
    # Read all csv files with the name of the task in them
    all_files = glob.glob(path + task + "*.csv")
    for filename in all_files:
      	
	# Read file, sort algos by alphabetic order and remove the '_' in their name
        frame = pd.read_csv(filename, delimiter = ',')
        frame = sort_frame(frame)
        for i in range (0, frame.shape[0]):
            for j in range (0, frame.shape[1]):
                if (isinstance(frame.iloc[i, j], str)):
                    frame.iloc[i, j] = add_space(frame.iloc[i, j])

        # List of variants and graphs for this file
        graphs = list(frame.columns.values)
        variant_list = list(frame["variant"].drop_duplicates().values) # List of variants for this task
        end_path = "_" + task + "_" + remove_prefixe(filename[size_path+len(task)+1:filename.find('.csv')]) + ".png"
        legend = True # Plot the legend only on the first graph of each type
	            
        # Pick correct color from data frame using the variant present in this frame
        palette = list(palette_list[palette_list["variant"].isin(variant_list)]['colors'].values)
        if (len(palette) > 0): #if palette not empty, set it
            sns.set_palette(palette)
            
            
        # Plot vs eval graphs
        if ("eval" in graphs):
            max_eval = frame.groupby("variant")["eval"].max().min() # Limit of the x axis
            frame = set_same_x(frame, variant_list, "eval", \
	    	variants[variants["task"] == task]) # Avoid x axis replication problem

            for item in graphs:
                if (item != "eval") and (item != "variant"): # Plot graph
                    plot_graph_from_csv(frame, "eval", "Number of evaluations", item, legend, max_eval, 
                            task, path + item + end_path)
                    if plot_one_legend_only: # If need legend on one graph of the csv only
                        legend = False
                    if compute_p_value: # Compute p-value
                        compute_p_values(frame, task, variant_list, item, path)
        

        # Plot vs gen graphs
        elif ("gen" in graphs):
            max_gen = frame.groupby("variant")["gen"].max().max() # Limit of the x axis
            frame = set_same_x(frame, variant_list, "gen",\
	    	variants[variants["task"] == task]) # Avoid x axis replication problem
           
            for item in graphs:
                if (item != "gen") and (item != "variant"): # Plot graph
                    plot_graph_from_csv(frame, "gen", "Number of generations", item, legend, max_gen, 
                            task, path + item + end_path)
                    if plot_one_legend_only: # If need legend on one graph of the csv only
                        legend = False
    
                        
print("Finish plotting. \n\n")
