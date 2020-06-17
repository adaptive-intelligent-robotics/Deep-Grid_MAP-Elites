#ifndef MAP_GRID
#define MAP_GRID

#include <boost/multi_array.hpp>

namespace sferes {
  namespace qd {
    namespace container {
      
      SFERES_STOCHASTIC_GRID(MAP_grid)
      {
      public:
	typedef boost::shared_ptr<Phen> indiv_t;
	typedef boost::shared_ptr<Cell> cell_t;


	// add to grid depending on type of experiment
	bool add(indiv_t indiv)
	{
	  this->_total_eval++;
	  if (this->_not_valid(indiv))
	    return false;
	  
	  // If first occurence of a cell, initialisation
	  auto behav_pos = this->get_index(indiv);
	  if (!this->_array(behav_pos))
	    {
	      this->_n_eval += indiv->fit().value_number();
	      this->_total_indiv++;
	      this->_array(behav_pos) = cell_t(new Cell()); //Create new vector
	      this->_array(behav_pos)->push_back(indiv); //Add individual
	      return true;
	    }

	  // Else, find its place in the cell
	  size_t rank = 0;
	  size_t rank_sup = this->_array(behav_pos)->size();
	  while (rank != rank_sup)
	    {
	      size_t rank_mid = (size_t) std::floor( ((float) (rank + rank_sup)) / 2 );
	      if (this->_compare_epsilon(indiv, this->_array(behav_pos)->operator[](rank_mid)))
		rank_sup = rank_mid;
	      else
		rank = rank_mid + 1;
	    }

	  // If before the end of the cell, add it in place and remove other indiv if full
	  if (rank<this->_array(behav_pos)->size())
	    {
	      this->_n_eval += indiv->fit().value_number();
	      this->_array(behav_pos)->insert(rank, indiv);
	      // remove last indiv if cell is full
	      if(this->_array(behav_pos)->size()>this->deep_max)
		{
		  this->_n_eval -= this->_array(behav_pos)->operator[](this->deep_max)->fit().value_number();
		  this->_array(behav_pos)->pop_back();
		  this->_total_indiv--;
		}
	      this->_total_indiv++;

	      return true;
	    }
	  
	  // If not add during the loop but cell not full, add at the end
	  if ((this->_array(behav_pos))->size()<this->deep_max)
	    {
	      this->_n_eval += indiv->fit().value_number();
	      (this->_array(behav_pos))->push_back(indiv);
	      this->_total_indiv++;

	      return true;
	    }
	  return false;
	};
      }; // MAP_grid
    } // namespace container
  } // namespace qd
} // namespace sferes
#endif
