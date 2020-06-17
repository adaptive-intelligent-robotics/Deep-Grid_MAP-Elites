#ifndef ALGOS_HPP_
#define ALGOS_HPP_

#include "qd_stochastic.hpp"

// Containers
#include "grid_stochastic.hpp"
#include "cell/cell.hpp"

#include "add_grid/naive.hpp"
#include "add_grid/baseline.hpp"
#include "add_grid/deep.hpp"
#include "add_grid/adaptive.hpp"
#include "add_grid/adaptive_bd.hpp"

// Selectors
#include "selector/deep_proportional.hpp"
#include "selector/deep_uniform.hpp"

namespace sferes {
  namespace qd {

    // Define the nature of the cells of the grid
    template <typename Phen, typename Params> using Cell = container::Cell<Phen, Params>;
    template <typename Phen, typename Params> using Cell_first = container::Cell_First_desc<Phen, Params>;

    // Map Elites
    template <typename Phen, typename Eval, typename Stat, typename Modifier, typename Params>
    using MAP = qd::QD_Stochastic<Phen, Eval, Stat, Modifier,
					   selector::Deep_uniform
					   	<Phen, Cell<Phen, Params>, Params>,
					   container::MAP_grid
					   	<Phen, Cell<Phen, Params>, Params>, 
					   Cell<Phen, Params>,
					   Params>;
    // Naive_sampling 
    template <typename Phen, typename Eval, typename Stat, typename Modifier, typename Params>
    using Naive_sampling = qd::QD_Stochastic<Phen, Eval, Stat, Modifier,
					   selector::Deep_uniform
					   	<Phen, Cell<Phen, Params>, Params>,
					   container::Naive_sampling_grid
					   	<Phen, Cell<Phen, Params>, Params>, 
					   Cell<Phen, Params>,
					   Params>;
    // Adaptive
    template <typename Phen, typename Eval, typename Stat, typename Modifier, typename Params>
    using Adaptive_MAP = qd::QD_Stochastic<Phen, Eval, Stat, Modifier,
					      selector::Deep_uniform
					      	  <Phen, Cell_first<Phen, Params>, Params>,
					      container::Adaptive_sampling_grid
					      	  <Phen, Cell_first<Phen, Params>, Params>, 
					      Cell_first<Phen, Params>,
					      Params>;
    // Adaptive_BD
    template <typename Phen, typename Eval, typename Stat, typename Modifier, typename Params>
    using Adaptive_BD_MAP = qd::QD_Stochastic<Phen, Eval, Stat, Modifier,
					      selector::Deep_uniform
					      	<Phen, Cell<Phen, Params>, Params>,
					      container::Behaviour_adaptive_sampling_grid
					      	<Phen, Cell<Phen, Params>, Params>, 
					      Cell<Phen, Params>,
					      Params>;
    // Deep_grid
    template <typename Phen, typename Eval, typename Stat, typename Modifier, typename Params>
    using Deep_grid = qd::QD_Stochastic<Phen, Eval, Stat, Modifier,
					   selector::Deep_disordered_proportional
					  	<Phen, Cell<Phen, Params>, Params>,
					   container::Deep_disordered_grid
					   	<Phen, Cell<Phen, Params>, Params>, 
					   Cell<Phen, Params>,
					   Params>;
  } // namespace qd
} // namespace sferes
#endif
