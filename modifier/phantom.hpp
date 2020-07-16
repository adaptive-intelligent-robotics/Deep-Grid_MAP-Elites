#ifndef MODIFIER_PHANTOM_HPP
#define MODIFIER_PHANTOM_HPP

namespace sferes {
  namespace modif {

    // Base class of the phantom modifiers used to compute the true values of the performance of each cell

    template<typename Params, typename Exact = stc::Itself>
    class Phantom_base
    {

    public:
      typedef std::vector<double> desc_t;
      
      // Constructor : resize all arrays to match container dimensions
      Phantom_base() 
      { 
	for (size_t i = 0; i < _dim; ++i)
	  _grid_shape[i] = Params::qd::grid_shape(i);
	_fitness.resize(_grid_shape);
	_moved_fitness.resize(_grid_shape);
	_moved.resize(_grid_shape);
	_clean();
      };

      // Apply the modifier
      template<typename Ea>
      void apply(Ea& ea) 
      {
       if (!_use_best_only || Params::stochastic::deep_size > 1)
	if (ea.gen() % Params::pop::dump_period == 0)
	  {
	    _clean();
	    _simulate_sample(ea);
	  }
      };

      // Get functions
      const double empty_value() const { return _empty_value; };

      template<typename Ea> const bool is_empty(Ea& ea, desc_t desc) const
      { return (_fitness(_find_index(ea, desc)) == _empty_value); };
      template<typename Ea> const bool is_moved_empty(Ea& ea, desc_t desc) const
      { return (_moved_fitness(_find_index(ea, desc)) == _empty_value); };

      template<typename Ea> const double get_fitness(Ea& ea, desc_t desc) const 
      { return _get_value(ea, desc, _fitness); };
      template<typename Ea> const double moved(Ea& ea, desc_t desc) const 
      { return _moved(_find_index(ea, desc)); };
      template<typename Ea> const double get_moved_fitness(Ea& ea, desc_t desc) const 
      { return _get_value(ea, desc, _moved_fitness); };

      const double original_quality() const {return _original_quality; };
      const double moved_quality() const {return _moved_quality; };

      const double mean_cell_move() const {return _mean_cell_move; };
      const double mean_move() const {return _mean_move; };
      
      const size_t moved_cells() const { return _N_non_static_cells; };
      const size_t static_cells() const { return _N_static_cells; };
      const double prop_moved_cells() const { return (double) _N_non_static_cells / (double) _N_filled_cells; };
      const double prop_static_cells() const { return (double) _N_static_cells / (double) _N_filled_cells; };
      const size_t filled_moved_cells() const { return _N_filled_moved_cells; };

    protected:
	
      // Use the whole cell indivs or the best individuals only
      bool _use_best_only;

      // Number of repet for modifier and number of dim BD
      size_t _n_repets;
      static const size_t _dim = Params::qd::behav_dim;

      // Define a constant to initialise grids and not confound it with actual fitness values
      const double _empty_value = -1;
      
      // Define array type
      typedef boost::multi_array<double, _dim> grid_t;
      typedef boost::array<typename grid_t::index, _dim> index_t;
      typedef boost::array<typename grid_t::index_range, _dim> range_t;
      index_t _grid_shape;

      // Define variables
      grid_t _moved;
      grid_t _fitness;
      grid_t _moved_fitness;

      double _original_quality;
      double _moved_quality;

      size_t _N_filled_cells;
      size_t _N_non_static_cells;
      size_t _N_static_cells;
      size_t _N_filled_moved_cells;

      double _mean_cell_move;
      double _mean_move;

      // Return the value of an array if not the empty value
      template<typename Ea> const double _get_value(Ea& ea, desc_t desc, grid_t grid) const
      {
        double value = grid(_find_index(ea, desc));
        return (value == _empty_value) ? 0.0 : value;
      };

      // Compute index in one of the grid from the descriptor values
      template<typename Ea> index_t _find_index(Ea& ea, desc_t desc) const
      {
	std::vector<double> coord = ea.container().get_coord(desc);
	index_t index;
	for (size_t i = 0; i < _dim; i++)
	  index[i] = round(std::min(1.0, coord[i]) * (_grid_shape[i] - 1));
	return index;
      };
      

      // Clean all variables of the modifier
      void _clean()
      {
	_original_quality = 0;
	_moved_quality = 0;
	_mean_cell_move = 0;
	_mean_move = 0;
	_N_non_static_cells = 0;
	_N_static_cells = 0;
	_N_filled_moved_cells = 0;
	for (double* i = _fitness.data(); i < (_fitness.data() + _fitness.num_elements()); i++)
	  *i = _empty_value;
	for (double* i = _moved_fitness.data(); i < (_moved_fitness.data() + _moved_fitness.num_elements()); i++)
	  *i = _empty_value;
	for (double* i = _moved.data(); i < (_moved.data() + _moved.num_elements()); i++)
	  *i = false;
      };

      
      // Simulate sample
      template<typename Ea>
      void _simulate_sample(Ea& ea)
      {
	// Read all filled cells
	auto filled_content = ea.container().get_filled_content();
	_N_filled_cells = filled_content.size();

	// Create all containers
	std::vector<double> quality (_N_filled_cells);
	std::vector<bool> non_static (_N_filled_cells);
	std::vector<double> cell_move (_N_filled_cells);
	std::vector<double> dist_move (_N_filled_cells);
	std::vector<bool> moved_dead (_N_filled_cells);
	std::vector<index_t> moved_index (_N_filled_cells);
	
	tbb::parallel_for(size_t(0), _N_filled_cells, size_t(1), [&](size_t cell_num)
	  {
	    // Corresponding cell
	    auto cell = filled_content[cell_num];

	    // Create the sample from the ea
	    auto sample = ea.sample(_n_repets, cell, _use_best_only);

	    // Information on this cell
	    desc_t original_desc = ea.container().get_desc(cell);
	    auto index = ea.container().get_index(cell);
	    size_t dim_desc = original_desc.size();
	    index_t original_index = _find_index(ea, original_desc);
	    
	    // Evaluate all samples
	    double fit = 0, dist_value = 0;
	    desc_t desc (dim_desc), mean_desc (dim_desc);
	    bool dead = false;
	    for (size_t indiv = 0; indiv < _n_repets; indiv++)
	      {
		// Simulate the indiv
		std::tie(fit, desc, dead) = sample[indiv]->fit().simulate(*sample[indiv]);
		
		// Compute dist stat
		dist_move[cell_num] = online_average(dist_move[cell_num], indiv, 
					ea.container().dist_indiv(original_desc, desc));
		
		// Compute fit and desc mean
		quality[cell_num] = online_average(quality[cell_num], indiv, fit);
		for (size_t j = 0; j < dim_desc; j++)
		  mean_desc[j] = online_average(mean_desc[j], indiv, desc[j]);
	      }
	    
	    // Set original stats
	    _fitness(original_index) = quality[cell_num];
	    
	    // If the mean indiv is out of grid, fill in moved stat but not moved grids
	    if (ea.container().is_out(mean_desc))
	      {
		moved_dead[cell_num] = true;
		_moved(original_index) = true;
		non_static[cell_num] = true;
		cell_move[cell_num] = ea.container().cell_dist(original_desc, mean_desc);
	      }
	    // Else, write the moved informations to fill in moved grid after
	    else 
	      {
		moved_dead[cell_num] = false;
		index_t moved_index_value = _find_index(ea, mean_desc);
		moved_index [cell_num] = moved_index_value;
		
		// If it has moved cell
		if (original_index != moved_index_value)
		  {
		    _moved(original_index) = true;
		    non_static[cell_num] = true;
		    cell_move[cell_num] = ea.container().cell_dist(original_desc, mean_desc);
		  }
		else
		  {
		    _moved(original_index) = false;
		    non_static[cell_num] = false;
		    cell_move[cell_num] = 0;
		  }
	      }
	    
	  });
	
	for (size_t cell_num = 0; cell_num < _N_filled_cells; cell_num++)
	  {
	    // Set original stats
	    _original_quality += quality[cell_num];
	    
	    // Set moved stats
	    if (non_static[cell_num])
	      _N_non_static_cells++;
	    else
	      _N_static_cells++;
	    _mean_cell_move += cell_move[cell_num];
	    _mean_move += dist_move[cell_num];
	    
	    // Set moved archive 
	    if (!moved_dead[cell_num])
	      {
		if (quality[cell_num] > _moved_fitness(moved_index[cell_num]))
		  {
		    // If the former fitness was the empty value then one more cell that is filled after moving
		    if (_moved_fitness(moved_index[cell_num]) == _empty_value)
		      _N_filled_moved_cells++;
		    _moved_quality += quality[cell_num] - _moved_fitness(moved_index[cell_num]);
		    _moved_fitness(moved_index[cell_num]) = quality[cell_num];
		  }
	      }
	  }
	_mean_cell_move /= (double)_N_filled_cells;
	_mean_move /= (double)_N_filled_cells;
      };

      double online_average(double mean, size_t N, double value)
      { return 1/(double)(N+1) * (value + (double)N * mean); };
      
    };

    // Phantom modifier
    template<typename Params, typename Exact = stc::Itself> class Phantom 
      : public Phantom_base<Params, typename stc::FindExact<Phantom<Params, Exact>, Exact>::ret>
    {
     public:
      Phantom()
      {
        this->_use_best_only = false;
        this->_n_repets = Params::stochastic::N_repets;
      };
    };

    template<typename Params, typename Exact = stc::Itself> class Phantom_best 
      : public Phantom_base<Params, typename stc::FindExact<Phantom_best<Params, Exact>, Exact>::ret>
    {
     public:
      Phantom_best()
      {
        this->_use_best_only = true;
        this->_n_repets = Params::stochastic::N_best_repets;
      };
    };
  }
}


#endif
