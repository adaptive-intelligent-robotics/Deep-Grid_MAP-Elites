#ifndef ADAPTIVE_SAMPLING_GRID
#define ADAPTIVE_SAMPLING_GRID

#include <tbb/tbb.h>

namespace sferes {
  namespace qd {
    namespace container {

      SFERES_STOCHASTIC_GRID(Adaptive_sampling_grid)
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
	  
	  auto behav_pos = this->get_index(indiv);
	  
	  // While indiv have not been evaluate the same number of time as the elite
	  while (!this->_array(behav_pos)
		 || (indiv->fit().value_number() 
		     < this->_array(behav_pos)->operator[](0)->fit().value_number()))
	    {
	      indiv->fit().eval(*indiv); // reevaluate indiv
	      this->_total_eval++;

	      // Check again if indiv needs to be added
	      if (this->_not_valid(indiv))
		return false;
	      
	      // if no elite in the cell, put it there
	      if (!this->_array(behav_pos))
		{
	          this->_n_eval += indiv->fit().value_number();
	          this->_total_indiv++;
	          this->_array(behav_pos) = cell_t(new Cell()); //Create new vector
	          this->_array(behav_pos)->push_back(indiv); //Add individual
		  return true;
		}
	      
	      // else, if the cell is full and indiv worste than last element
	      if ((this->_array(behav_pos)->size()==this->deep_max)
		  && !this->_compare_epsilon(indiv, 
					     (this->_array(behav_pos))->operator[](this->deep_max-1)))
		{
		  //reevaluate all elites
		  this->_total_eval += this->deep_max;
		  tbb::parallel_for(size_t(0), this->deep_max, size_t(1), [&](size_t rank)
				    {
				      this->_array(behav_pos)->operator[](rank)->fit()
					.eval(*this->_array(behav_pos)->operator[](rank));
				    });

		  //reorganise the cell
		  size_t rank = 0;
		  size_t max_rank = this->deep_max;
		  while(rank < max_rank)
		    {
		      //if the elites does not need to be added anymore
	      	      if (this->_not_valid(this->_array(behav_pos)->operator[](rank)))
			{
			  this->_n_eval -= this->_array(behav_pos)->operator[](rank)->fit().value_number();
			  this->_total_indiv--;
			  this->_array(behav_pos)->erase(rank);
			  rank--;
			  max_rank = this->_array(behav_pos)->size();
			  
			  // if the cell is now empty, add the indiv
			  if (max_rank == 0)
			    {
			      this->_n_eval += indiv->fit().value_number();
			      this->_array(behav_pos)->push_back(indiv);
			      this->_total_indiv++;
			      
			      return true;
			    }
			}
		      
		      //else replace it where it belongs if needed
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
		      && (!this->_compare_epsilon(indiv, (this->_array(behav_pos))->operator[](this->deep_max-1))))
		      return false;
		}
	    }
	  // If the elites and the indiv have been evaluated the same number of time
	  // find where it should be added in the archive, using dichotomy
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
	  
	  // If archive not full, add it anyway
	  if ((this->_array(behav_pos))->size()<this->deep_max)
	    {
	      this->_n_eval += indiv->fit().value_number();
	      this->_array(behav_pos)->insert(rank, indiv);
	      this->_total_indiv++;
	      return true;
	    }
	  
	  // If archive full and the indiv is better than the last one, add it where it belongs
	  if (rank<this->_array(behav_pos)->size())
	    {
	      this->_n_eval += indiv->fit().value_number();
	      this->_array(behav_pos)->insert(rank, indiv);
	      this->_n_eval -= this->_array(behav_pos)->operator[](this->deep_max)->fit().value_number();
	      this->_array(behav_pos)->pop_back();
	      return true;
	    }
	  return false;
	};
	
      }; // Adaptive_sampling_grid
    } // namespace container
  } // namespace qd
} // namespace sferes
#endif
