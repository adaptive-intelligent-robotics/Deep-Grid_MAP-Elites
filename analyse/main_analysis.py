import argparse
import time

from read_task import read_tasks
from write_files import write_replications
from graph import graph
from archive import archive

# Main
if __name__ == "__main__":

    # Read inputs
    parser = argparse.ArgumentParser(description="Analysis of all results in folder")
    parser.add_argument("--directory", "-d", type=str, 
                        help="Directory containing all results to analyse", 
                        default="/git/sferes2/build/exp/rastrigin_stochastic")
    parser.add_argument("--p_value", "-p", action="store_true", 
                        help="Compute p-value of all statistics", 
                        default=False)
    parser.add_argument("--legend", "-l", action="store_false", 
                        help="Plot legend on each graph", 
                        default=True)
    args = parser.parse_args()

    # Create task, algo, stat and path dataframe for analysis
    print("\nRead tasks.dat files")
    task_frame, algo_frame, repl_frame, stat_frame, path_frame = read_tasks(args.directory)
    print("Finish reading, overview:")
    print(repl_frame)
    print("\n")

    # Write report.md and task files
    print('Writting overview of experiments')
    write_replications(args.directory, repl_frame)
    print('End writting\n')

    # Ploting all graphs
    for task in task_frame['task'].drop_duplicates().values:
        taskf = task_frame[task_frame['task'] == task]
        print("\nTask: ", task)
        for stat in stat_frame[ stat_frame['task'] == task ]['stat'].drop_duplicates().values:

            statf = stat_frame[(stat_frame['task'] == task) & (stat_frame['stat'] == stat)] \
                              [['replications', 'algo']].merge(algo_frame, on='algo', how='left')
            pathf = path_frame[(path_frame['task'] == task) & (path_frame['stat'] == stat)]
            type_stat = stat_frame[ stat_frame['stat'] == stat]['type'].iloc[0]

            # If graph type
            if type_stat == 'graph':
                print("\nPrinting graph")
                start_time = time.time()
                graph(args.directory, task, statf, pathf, args.p_value, args.legend)
                print("Time: %s seconds" % (time.time() - start_time))

            # If archive type
            if type_stat == 'archive':
                print("\nPrinting archive")
                start_time = time.time()
                archive(args.directory, task, taskf, statf, pathf)
                print("Time: %s seconds" % (time.time() - start_time))

    print("End ploting \n")


