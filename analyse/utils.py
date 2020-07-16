import pandas as pd
from matplotlib.colors import ListedColormap
import math

# Remove the prefixe from a name
def remove_prefixe(name):
    prefixes = ["cell_fitness_", "cell_stability_", "best_fitness_", "best_stability_", "filling_"]
    for prefixe in prefixes:
        if prefixe in name:
            return name[len(prefixe):]
    return name
                
# Change between _ and space
def add_space(name):
        return name.replace("_", " ")
def remove_space(name):
        return name.replace(" ", "_")

# Add space in a whole dataframe
def add_space_frame(frame):
    clean_frame = frame.copy()
    for i in range (0, clean_frame.shape[0]):
        for j in range (0, clean_frame.shape[1]):
            if (isinstance(clean_frame.iloc[i, j], str)):
                clean_frame.iloc[i, j] = add_space(clean_frame.iloc[i, j])
    return clean_frame

# Add space in all elements of a list
def add_space_list(list_name):
    for i in range (len(list_name)):
        list_name[i] = add_space(list_name[i])
    return list_name

# Sort a frame in alphabetic order of algorithms
def sort_frame(frame):
    return pd.concat([frame[frame["algo"].str.contains('Baseline')], \
                            frame[~frame["algo"].str.contains('Baseline')].sort_values(by="algo")])

# Compute the AIRL colormap
def AIRL_colormap():
    color = []
    for i in range(100):
        x = i / 100.0
        color.append([(math.sin(x*2*math.pi*0.9 + 1.3*math.pi) / 2.0+0.5)*0.8+0.1,
                  math.sin(x*2*math.pi*0.9)/2*0.6+0.60,
                  1-(math.sin(x*2*math.pi*0.9+1.6*math.pi)/2+0.5)] )
    return ListedColormap(color)

