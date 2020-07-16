import os

# Write the stats file for each task use in final report.md
def write_replications(path, frame):
    for task in frame['task'].drop_duplicates().values:
        with open(os.path.join(path, 'stats_' + task + '.dat'), "w") as f:
            f.write(str(frame[frame['task'] == task]['replications'].sum()) + ' algos \n')
            for algo in frame[frame['task'] == task]['algo'].drop_duplicates().values:
                f.write(str(frame[(frame['task'] == task) & (frame['algo'] == algo)]['replications'].sum()) \
                        + ' ' + algo + '\n')
