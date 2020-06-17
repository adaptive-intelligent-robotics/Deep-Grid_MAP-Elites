#include <sferes/eval/parallel.hpp>

// Rastrigin
#ifdef RASTRIGIN
#define BEST 1 // Add analysis of the best indiv of each cells for all algo using a depth
#include "tasks/standard_functions_stochastic.hpp"
tbb::task_scheduler_init init(8);
#endif

// Hexa circle
#ifdef HEXA_OMNI
#define POLAR_COORD 1 // Use polar coordinate archive instead of cartesian coordinate one
#define BEST 1 // Add analysis of the best indiv of each cells for all algo using a depth
#include "tasks/hexapod_omnidirectional_stochastic.hpp"
tbb::task_scheduler_init init(32);
#endif

// Arm var
#ifdef ARM_VAR
#define POLAR_COORD 1 // Use polar coordinate archive instead of cartesian coordinate one
#define BEST 1 // Add analysis of the best indiv of each cells for all algo using a depth
#include "tasks/redundant_arm_stochastic.hpp"
tbb::task_scheduler_init init(8);
#endif

// Main
#include "deep_grid/algos.hpp"
#include "modifier/modifier.hpp"
#include "stat/stat_stochastic.hpp"
#include "run_utils/read_input.hpp"
#include "run_utils/run.hpp"
int main(int argc, char **argv)
{
  using namespace sferes;
  fit_stochastic::task::load_and_init_robot();

  // Process inputs
  using Params = fit_stochastic::params_stochastic::Params;
  typedef eval::Parallel<Params> eval_t;
  if (argc>1)
    read_input<Params>(argc, argv);
  print_task<Params>();

  // Run baseline (ie compute only baseline-relevant stats)
  if (Params::algos::Truth)
    {
      run_baseline<
          fit_stochastic::baseline_max::phen_t<Params>,
	  eval_t, 
	  stat_list::stat_baseline_t<Params, fit_stochastic::baseline_max::phen_t<Params> >, 
	  modifier_list::modifier_t<Params>, 
	  Params> ("Baseline_noise_free");
      Params::algos::Truth = false;
    }
  
  // Run the task
  run_experiments<
      fit_stochastic::task::phen_t<Params>,
      eval_t, 
      stat_list::stat_t<Params, fit_stochastic::task::phen_t<Params> >, 
      modifier_list::modifier_t<Params>,
      Params> ();
  
  fit_stochastic::task::reset_robot();
  return 0;
};
