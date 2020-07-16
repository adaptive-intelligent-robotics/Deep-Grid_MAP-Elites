import glob
import os
import pandas as pd
from seaborn import husl_palette

from utils import sort_frame

##################### Read files ########################

# Find all tasks files
def find_tasks(path):
    return [os.path.join(root, name) for root, dirs, files in os.walk(path) for name in files \
                                                                            if name.endswith("tasks.dat")]

# Separate all valuable informations of the line of a file
def read_stats(line):
    for stat in line.rstrip().split(' '):
        values = stat.rstrip().split(':')
        if values[0] == "Task":
            task = str(values[1])
        elif values[0] == "Algo":
            algo = str(values[1])
        elif values[0] == "Radius":
            radius = float(values[1])
        elif values[0] == "BD":
            bd = [int(value) for value in values[1].rstrip().split(',') if value]
        elif values[0] == "Stats":
            stats = [str(value) for value in values[1].rstrip().split(',') if value]
        elif values[0] == "Types":
            types = [str(value) for value in values[1].rstrip().split(',') if value]
    assert len(stats) == len(types)
    return task, algo, radius, bd, stats, types

# Fill in the frame for one line
def read_line(taskfile, task_frame, repl_frame, stat_frame, path_frame, line):

    task, algo, radius, bd, stats, types = read_stats(line)
    path = os.path.join(taskfile[:-len("tasks.dat")], algo)

    if not(task in task_frame.values): # Add task to frame if not already in it
        task_frame = task_frame.append({'task' : task,
                                        'radius' : radius,
                                        'bd' : bd},
                                        ignore_index=True)
    if not(algo in repl_frame[repl_frame['task'] == task].values): # Add task-algo to repl if not already in it
        repl_frame = repl_frame.append({'task' : task,
                                        'algo' : algo,
                                        'replications' : 1},
                                        ignore_index=True)
    else:
        repl_frame.at[ repl_frame[(repl_frame['algo'] == algo) & (repl_frame['task'] == task)].index[0], 
                       'replications'] += 1
    for i in range (len(stats)): # Add stats except if replication
        if stat_frame[ (stat_frame['algo'] == algo) & \
                       (stat_frame['task'] == task) & \
                       (stat_frame['stat'] == stats[i]) ].empty:
            stat_frame = stat_frame.append({'task' : task,
                                            'algo' : algo,
                                            'stat' : stats[i],
                                            'type' : types[i],
                                            'replications' : 1},
                                            ignore_index=True)
        else:
            stat_frame.at[ stat_frame[ (stat_frame['algo'] == algo) & \
                                       (stat_frame['task'] == task) & \
                                       (stat_frame['stat'] == stats[i]) ].index[0], 
                           'replications'] += 1

    for i in range (len(stats)): # Add a path for each stat file
        path_frame = path_frame.append({'task' : task,
                                        'algo' : algo,
                                        'stat' : stats[i],
                                        'path' : os.path.join(path, stats[i])},
                                        ignore_index=True)
    return task_frame, repl_frame, stat_frame, path_frame

################ Main functions ########################

# Return all frames found from all tasks files
def read_tasks(path):

    task_frame = pd.DataFrame(columns = ["task", "radius", "bd"])
    repl_frame = pd.DataFrame(columns = ["task", "algo", "replications"])
    stat_frame = pd.DataFrame(columns = ["task", "algo", "stat", "type", "replications"])
    path_frame = pd.DataFrame(columns = ["task", "algo", "stat", "path"])

    # Go through all lines of all task files
    taskfiles = find_tasks(path)
    for taskfile in taskfiles:
       with open(taskfile) as f:
            for line in f:
                task_frame, repl_frame, stat_frame, path_frame = read_line(taskfile, task_frame, repl_frame,
                                                               stat_frame, path_frame, line)

    # Create algo / color frame
    variants = sort_frame(stat_frame)['algo'].drop_duplicates().values
    algo_frame = pd.DataFrame(data = {'algo' : variants,
                                      'colors' : husl_palette(len(variants), l=0.7)})

    return task_frame, algo_frame, repl_frame, stat_frame, path_frame

