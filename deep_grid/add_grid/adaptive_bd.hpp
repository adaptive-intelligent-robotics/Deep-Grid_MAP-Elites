#ifndef BEHAVIOUR_ADAPTIVE_SAMPLING_GRID
#define BEHAVIOUR_ADAPTIVE_SAMPLING_GRID
#include <tbb/tbb.h>

namespace sferes {
  namespace qd {
    namespace container {

      SFERES_STOCHASTIC_GRID(Behaviour_adaptive_sampling_grid)
      {
      public:
	typedef boost::shared_ptr<Phen> indiv_t;
	typedef boost::shared_ptr<Cell> cell_t;
	static const size_t dim = Params::qd::behav_dim;


	// add to grid depending on type of experiment
	bool add(indiv_t indiv)
	{
	  this->_total_eval++;
	  if (this->_not_valid(indiv))
	    return false;

	  // Create all needed variables
	  auto behav_pos = this->get_index(indiv);
	  boost::multi_array<size_t, dim> visit_count;
	  visit_count.resize(this->grid_shape);
	  visit_count(behav_pos)++;
	  size_t total_visit = 0;
	  
	  // While indiv have not been evaluate the same number of time as the elite
	  while ((!this->_array(behav_pos) || this->_array(behav_pos)->size() == 0)
		 || (indiv->fit().value_number() 
		     < this->_array(behav_pos)->operator[](0)->fit().value_number()))
	    {
	      // Reevaluate it
	      indiv->fit().eval(*indiv); // reevaluate indiv
	      this->_total_eval++;
	      behav_pos = this->get_index(indiv); //reevaluate descriptor
	      visit_count(behav_pos)++; //count number of evaluation of each cell
	      total_visit++;

	      // Check again if indiv needs to be added
	      if (this->_not_valid(indiv))
		return false;

	      // If the cell is empty, add it
	      if (!this->_array(behav_pos) || this->_array(behav_pos)->size() == 0)
		{
	          this->_n_eval += indiv->fit().value_number();
	          this->_total_indiv++;
		  if (!this->_array(behav_pos))
	            this->_array(behav_pos) = cell_t(new Cell()); //Create new vector
	          this->_array(behav_pos)->push_back(indiv); //Add individual
		  return true;
		}
	      
	      // If the cell is full and it is worste than the last element
	      if ((this->_array(behav_pos)->size()==this->deep_max)
		  && !this->_compare_epsilon(indiv, 
					     (this->_array(behav_pos))->operator[](this->deep_max-1)))
		{
		  // Reevaluate all elites
	  	  this->_total_eval += this->deep_max;
		  tbb::parallel_for(size_t(0), this->deep_max, size_t(1), [&](size_t rank)
				    {
		      this->_array(behav_pos)->operator[](rank)->fit()
			.eval(*this->_array(behav_pos)->operator[](rank));
		    });

		  // Reorganise the elites
		  size_t rank = 0;
		  size_t max_rank = this->deep_max;
		  while (rank < max_rank)
		    {
		      // If the elites does not need to be added anymore, remove it
	      	      if (this->_not_valid(this->_array(behav_pos)->operator[](rank)))
			{
			  this->_n_eval -= this->_array(behav_pos)->operator[](rank)->fit().value_number();
			  this->_total_indiv--;
			  this->_array(behav_pos)->erase(rank);
			  rank--;
			  max_rank = this->_array(behav_pos)->size();
			  
			  // If the cell is now empty, add the indiv
			  if (max_rank == 0)
			    {
			      this->_n_eval += indiv->fit().value_number();
			      this->_array(behav_pos)->push_back(indiv);
			      this->_total_indiv++;
			      return true;
			    }
			}
		      
		      // If the elite needs to move outside the cell
		      else if (this->get_index(this->_array(behav_pos)->operator[](rank)) != behav_pos)
			{
			  indiv_t elite = this->_array(behav_pos)->operator[](rank);
	      		  this->_n_eval -= elite->fit().value_number();
			  this->_array(behav_pos)->erase(rank);
	                  this->_total_indiv--;
			  add(elite);
			  rank--;
			  max_rank = this->_array(behav_pos)->size();
			  if (max_rank == 0)
			    {
	      		      this->_n_eval += indiv->fit().value_number();
			      this->_array(behav_pos)->push_back(indiv);
	          	      this->_total_indiv++;
			      return true;
			    }
			}

		      // If the elite needs to move inside the cell
		      else if (rank>0)
			{
			  size_t i = rank; //swap elements until it reaches the place it belongs to
			  while (i>0 
				 && this->_compare_epsilon(this->_array(behav_pos)->operator[](i), 
							   this->_array(behav_pos)->operator[](i-1)))
			    {
			      this->_array(behav_pos)->swap(i, i-1);
			      i--;
			    }
			}
		      rank++;
		    }

		  // If after all reevaluations, the cell is still full and the last elite is still better
		  if ((this->_array(behav_pos)->size()==this->deep_max)
		      && (!this->_compare_epsilon(indiv, 
						  (this->_array(behav_pos))->operator[](this->deep_max-1))))
		      return false;
		}
	    }
	  // If thay have been evaluated the same number of time
	  // AND it is than the last element of the cell and mainly evaluate in this cell
	  if ((this->_array(behav_pos)->size()<=this->deep_max
	       || this->_compare_epsilon(indiv, (this->_array(behav_pos))->operator[](this->deep_max-1)))
	      && visit_count(behav_pos)>total_visit/2)
	    {
	      // Find where to add it in the cell
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
	      
	      // Add it there
	      this->_n_eval += indiv->fit().value_number();
	      this->_array(behav_pos)->insert(rank, indiv);
	      
	      // If the cell is full, delete the last one
	      if ((this->_array(behav_pos))->size()>this->deep_max)
		{
		  this->_n_eval -= this->_array(behav_pos)->operator[](this->deep_max)->fit().value_number();
		  this->_array(behav_pos)->pop_back();
		  this->_total_indiv--;
		}
	      this->_total_indiv++;
	      return true;
	    }
	  return false;
	};
      }; // Behaviour_adaptive_sampling_grid
    } // namespace container
  } // namespace qd
} // namespace sferes
#endif
