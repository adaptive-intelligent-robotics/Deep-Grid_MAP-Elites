#ifndef QD_CONTAINER_STOCHASTIC_GRID_HPP
#define QD_CONTAINER_STOCHASTIC_GRID_HPP

#include <boost/multi_array.hpp>

namespace sferes {
  namespace qd {
    namespace container {
      template <typename Phen, typename Cell, typename Params> class Stochastic_grid {
	
      public:

	typedef boost::shared_ptr<Phen> indiv_t;
	typedef typename std::vector<indiv_t> pop_t;

	const size_t deep_max = Params::stochastic::deep_size;
	typedef boost::shared_ptr<Cell> cell_t;

	typedef std::vector<double> desc_t;
	typedef std::vector<double> coord_t;
				
	// Define grid as multiarray of cells of size dim
	static const size_t dim = Params::qd::behav_dim;
	typedef boost::multi_array<cell_t, dim> array_t;
	typedef typename array_t::multi_array_base::index_range index_range_t;
	typedef boost::detail::multi_array::index_gen<dim, dim> index_gen_t;
	typedef typename array_t::template const_array_view<dim>::type view_t;
	typedef boost::array<typename array_t::index, dim> behav_index_t;
	behav_index_t grid_shape;

	// Constructor                
	Stochastic_grid()
	{
	  assert(dim == Params::qd::grid_shape_size());
	  for (size_t i = 0; i < Params::qd::grid_shape_size(); ++i)
	    grid_shape[i] = Params::qd::grid_shape(i);
	  _array.resize(grid_shape);
	  _cell = cell_t(new Cell());
	};


	// Get array dimensions
	const size_t* get_size_grid() const { return _array.shape(); }; 
	const size_t get_number_cells() const {return _array.num_elements(); };


	// Get information on filling of the array
	size_t current_eval() const { return _n_eval; };
	size_t total_eval() const { return _total_eval; };
	size_t total_indiv() const { return _total_indiv; };
	
	
	// Get full content of the array
	const array_t& archive() const { return _array; };
	void get_full_content(pop_t& content) const
	{
	  for (const cell_t* i = _array.data();
	       i < (_array.data() + _array.num_elements()); ++i)
	    if (*i)
	      for(size_t j=0; j<(*i)->size(); j++)
		content.push_back((*i)->operator[](j));
	};
	
	
	// Get filled content of the array
	void get_filled_content(std::vector<cell_t>& content) const
	{
	  for (const cell_t* i = _array.data(); i < (_array.data() + _array.num_elements()); ++i)
	    if (*i)
	      content.push_back(*i);
	};
	std::vector<cell_t> get_filled_content() const
	{
	  std::vector<cell_t> filled_content;
	  get_filled_content(filled_content);
	  return filled_content;
	};
	
	// Return if a cell is filled or not
	template <typename Indiv> bool is_filled(Indiv ind) const
	{
	  if (_array(get_index(ind)))
	    return true;
	  else
	    return false;
	};
	
	// Get the index in the array
	template <typename Indiv> behav_index_t get_index(const Indiv ind) const
	{
	  coord_t coord = _coord(ind);
	  behav_index_t behav_pos;
	  for (size_t i = 0; i < Params::qd::grid_shape_size(); ++i)
	    behav_pos[i] = round(std::min(1.0, coord[i]) * (grid_shape[i] - 1));
	  return behav_pos;
	};

	// Get one cell of the array
	template <typename Indiv> cell_t get_cell(const Indiv ind) const { return _array(get_index(ind)); };
	
	// Get limits of a cell
	template <typename Indiv> std::tuple<desc_t, desc_t> get_cell_limits(const Indiv ind) const 
	{ 
	  coord_t coord = _coord(ind);
	  desc_t min (Params::qd::grid_shape_size());
	  desc_t max (Params::qd::grid_shape_size());
	  for (size_t i = 0; i < Params::qd::grid_shape_size(); ++i)
	  {
	    size_t index = round(coord[i] * (grid_shape[i] - 1));
	    min[i] = ((double)index - 0.5) / (grid_shape[i] - 1);
	    max[i] = ((double)index + 0.5) / (grid_shape[i] - 1);
	  }
	  return {min, max};
	};

	// Get coordinates of the center of a cell
	template <typename Indiv> desc_t get_center(const Indiv ind) const { return _center(ind); };

	// Get distance between two individuals
	template <typename Indiv> float dist_indiv(const Indiv ind1, const Indiv ind2) const 
	{ return _distance(_desc(ind1), _desc(ind2)); };

	// Get distance to the center of the corresponding cell
	template <typename Indiv> float dist_center(const Indiv ind) const 
	{ return _distance(_desc(ind), _center(ind)); };

	// Get distance to a specified cell center
	template <typename Indiv, typename Center> float dist_center(const Indiv ind, const Center center) const 
	{ return _distance(_desc(ind), _center(center)); };

	// Get distance in number of cells
	template <typename Indiv1, typename Indiv2> size_t cell_dist(const Indiv1 ind1, const Indiv2 ind2) const 
	{ return _cell->get_cell_difference(_desc(ind1), _desc(ind2)); };
	
	// Test if out of grid
	template <typename Indiv> bool is_out(const Indiv ind) const { return _cell->is_out(ind); };

	// Return the coordinate of an indiv
	template <typename Indiv> coord_t get_coord(const Indiv ind) const { return _coord(ind); };

	// Return the descriptor of an indiv
	template <typename Indiv> desc_t get_desc(const Indiv ind) const { return _desc(ind); };

	// update population of parent and offspring using update novelty et update indiv
	void update(pop_t& offspring, pop_t& parents) { _update_cells(offspring); };


      protected:
        
	array_t _array;
	size_t _n_eval = 0; //Number of eval currently in the grid
	size_t _total_eval = 0; //Total number of evals used
	size_t _total_indiv = 0; //Total number of indivs currently in the grid

	cell_t _cell; // An example cell used to call needed methods

	// Compute a distance between two vectors
	template <typename Numeric> 
	  float _distance(const std::vector<Numeric> vec1, const std::vector<Numeric> vec2) const
	{
	  assert(vec1.size() == vec2.size());
	  float dist = 0.0f;
	  for (size_t i = 0; i < vec1.size(); ++i)
	    dist += pow(std::min(1.0f, (float)vec1[i]) - std::min(1.0f, (float)vec2[i]), 2);
	  dist = sqrt(dist);
	  return dist;
	};

	// Get coordinate of a cell
 	coord_t _coord(const cell_t cell) const { return cell->get_coord(); };
	coord_t _coord(const behav_index_t behav_pos) const { return _array(behav_pos)->get_coord(); };
	coord_t _coord(const indiv_t& indiv) const { return _cell->get_coord(indiv); };
	coord_t _coord(const desc_t desc) const { return _cell->get_coord(desc); };

	// Get desc of a cell
 	desc_t _desc(const cell_t cell) const { return cell->get_desc(); };
	desc_t _desc(const behav_index_t behav_pos) const { return _array(behav_pos)->get_desc(); };
	desc_t _desc(const indiv_t& indiv) const { return _cell->get_desc(indiv); };
	desc_t _desc(const desc_t desc) const { return desc; };

	// Get center of a cell
 	desc_t _center(const cell_t cell) const { return cell->get_center(); };
	desc_t _center(const behav_index_t behav_pos) const { return _array(behav_pos)->get_center(); };
	desc_t _center(const indiv_t& indiv) const { return _cell->get_center(indiv); };
	desc_t _center(const desc_t desc) const { return _cell->get_center(desc); };

	// Espilon add test, used by all add functions
	bool _compare_epsilon(indiv_t indiv1, indiv_t indiv2)
	{ return (indiv1->fit().value() > indiv2->fit().value()); };

	// Test is an indiv should be added to the grid
	bool _not_valid(const indiv_t indiv) const
	{ 
	  if (_cell->is_out(indiv))
	    indiv->fit().out_of_container();
	  return indiv->fit().dead(); 
	};


	// Update the cells of the grid
	void _update_cells(pop_t& offspring)
	{
	  std::vector<behav_index_t> memory; //memory of cells already reorganised
	  // Go through all offspring, find corresponding cell and refresh it
	  for (auto indiv : offspring)
	    {
	      if (!indiv->fit().dead())
		{
		  behav_index_t behav_pos = get_index(indiv);
		  if (std::find(memory.begin(), memory.end(), behav_pos) == memory.end())
		    _array(behav_pos)->update_cell();
		  memory.push_back(behav_pos);
		}
	    }
	};
	
      }; // stochastic_grid
    } // namespace container
  } // namespace qd
} // namespace sferes
// Define macro for adds

#define SFERES_STOCHASTIC_GRID(Class)					\
	  template <typename Phen, typename Cell, typename Params> \
	    class Class : public Stochastic_grid<Phen, Cell, Params>

#endif
