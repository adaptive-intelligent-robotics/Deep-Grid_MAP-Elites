#ifndef STAT_STOCHASTIC_HPP_
#define STAT_STOCHASTIC_HPP_

#include <sferes/stat/stat.hpp>
#include <sferes/fit/fitness.hpp>
#include <chrono>

// Archive filling
#include "filling/filling.hpp"
#include "filling/moved_filling.hpp"

// Cell analysis
#include "fitness/total_fitness.hpp"
#include "fitness/fitness_archive.hpp"
#include "stability/total_stability.hpp"

// Best individual analysis
#ifdef BEST
#include "fitness/best_fitness_archive.hpp"
#endif

namespace sferes
{
  namespace stat_list
  {
    template<typename Params, typename phen_t>
    using stat_baseline_t = boost::fusion::vector<
      // filling stats
      stat::Filling_cell<phen_t, Params>,
      stat::Filling_cell_moved<phen_t, Params>,

      // fitness stats
      stat::Archive_original_save<phen_t, Params>,
      stat::Total_quality<phen_t, Params>,

      // stability stats
      stat::Total_stability<phen_t, Params>
	      >;
    
    template<typename Params, typename phen_t>
    using stat_t = boost::fusion::vector<
      // filling stats
      stat::Filling_cell<phen_t, Params>,
      stat::Filling_cell_moved<phen_t, Params>,

#ifdef BEST
      // fitness best stats
      stat::Archive_best_original_save<phen_t, Params>,
      stat::Archive_best_moved_save<phen_t, Params>,
#endif
      // fitness stats
      stat::Archive_original_save<phen_t, Params>,
      stat::Archive_moved_save<phen_t, Params>,
      stat::Total_quality<phen_t, Params>,

      // stability stats
      stat::Total_stability<phen_t, Params>
	      >;
    
  };
};

#endif
