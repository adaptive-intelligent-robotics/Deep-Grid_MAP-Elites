
# Fast and stable MAP-Elites in noisy domains using deep grids

[![https://www.singularity-hub.org/static/img/hosted-singularity--hub-%23e32929.svg](https://www.singularity-hub.org/static/img/hosted-singularity--hub-%23e32929.svg)](https://singularity-hub.org/collections/4459)

This repository contains the code associated with [Fast and stable MAP-Elites in noisy domains using deep grids](url)

This code proposes an implementation of several MAP-Elites variants to handle uncertainty: the Explicit-averaging approach,  the two Adaptive-sampling approaches proposed in [MAP-Elites for noisy domains by  adaptive sampling](https://dl.acm.org/doi/abs/10.1145/3319619.3321904) and the DG-MAP-Elites approach. It allows to compare these approaches on the three tasks described in the paper.


# Libraries and dependencies

The implementation of all tasks and algorithms is based on the qd-branch of the C++ [Sferes2](https://github.com/sferes2/sferes2)  library presented in [Sferesv2: Evolvin' in the multi-core world](https://ieeexplore.ieee.org/abstract/document/5586158/?casa_token=EhBJLkircvMAAAAA:ls8I90Y5H2vsJk5RxCYs8X1T9yZHDhDEz5S6g5gatOzETle1LK_ib8zwodx6t5J_-Uwq_YP9), and the hexapod  control task uses the Dart simulator introduced in [Dart: Dynamic animation and robotics toolkit](https://joss.theoj.org/papers/10.21105/joss.00500.pdf).
Furthermore, the analysis of the results is based on [Panda](https://pandas.pydata.org/), [Matplotlib](https://matplotlib.org/) and [Seaborn](https://seaborn.pydata.org/index.html) libraries.

# Structure

The main part of the code contains the following files and folders:
- `deep_grid` contains the main structure of all algorithms based on the structure of QD algorithms introduced in [Quality and diversity optimization: A  unifying modular  framework](https://ieeexplore.ieee.org/abstract/document/7959075/). The approaches differ by the nature of their selector, the add-function of their container and, in the specific case of Adaptive sampling, the descriptor they are using for the individuals, which is implemented in the structure of the cells of the grid.
- `task` contains a re-definition of the qd fitness from the Sferes2 library which allow to handle re-sampling of the solutions. It also contains an implementation of each of the three tasks used in the paper, build on top of the original deterministic tasks used in previous works.
- `modifier` defines the procedure to evaluate the "true" value of fitness and behaviour descriptor of each cell as described in the paper.
- `stat` implements all the stats used to compare the algorithms.
- `run_utils` contains utils function used to read the options of each run and run all algorithms.

In addition, the `analysis` folder is used for the analysis, the `waf_tools` and `wscript` files for compilation, and the `Singularity` file to compile the Singularity container for this experiment.

# Execution

The results of the paper can be reproduced by running the Singularity container image of the experiment. 

This container contains an `app` for each approach-task combination, defines in the `Singularity` file:
- `Truth_arm_var`, `Truth_rastrigin`, `Truth_hexa`: Noise-free Baseline for each of the three tasks.
- `Naive_1_arm_var`, `Naive_1_rastrigin`, `Naive_1_hexa`: Explicit-averaging approach with 1 re-sampling.
- `Naive_50_arm_var`, `Naive_50_rastrigin`, `Naive_50_hexa`: Explicit-averaging approach with 50 re-sampling.
- `Adapt_arm_var`, `Adapt_rastrigin`, `Adapt_hexa`: Adaptive sampling approach without drifting elites.
- `Adapt_BD_10_arm_var`, `Adapt_BD_10_rastrigin`, `Adapt_BD_10_hexa`: Adaptive sampling approach with drifting elites.
- `Deep_50_arm_var`, `Deep_50_rastrigin`, `Deep_50_hexa`: DG-MAP-Elites approach with depth 50.
- `Analysis`: apps to generate the graphs and container plots of all variants and tasks which results are stored in the folder given as input.

A given app can be run with the following command: `singularity run --app *app_name* *image_name*`. All run-parameters are defined inside the apps, and the results of the execution are solved in a `results` folder, outside of the image, at the same location.

This container can be recompile using the files in the `Singularity` file.
