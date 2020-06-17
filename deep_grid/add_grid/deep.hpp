#ifndef DEEP_DISORDERED_GRID
#define DEEP_DISORDERED_GRID

namespace sferes {
  namespace qd {
    namespace container {

      SFERES_STOCHASTIC_GRID(Deep_disordered_grid)
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
	  
	  size_t size = this->_array(behav_pos)->size();
	  
	  // If cell not full, add it at the end
	  if (size < this->deep_max)
	    {
	      this->_n_eval += indiv->fit().value_number();
	      this->_array(behav_pos)->push_back(indiv);
	      this->_total_indiv++;
	    }
	  // Else choose a random indiv already in cell and add it in place
	  else
	    {
	      size_t index = misc::rand<size_t>(size);
	      this->_array(behav_pos)->replace(index, indiv);
	    }
	  return true;
	};

      }; // Deep_grid
    } // namespace container
  } // namespace qd
} // namespace sferes
#endif
