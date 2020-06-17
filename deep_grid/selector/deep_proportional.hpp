#ifndef QD_DEEP_DISORDERED_PROP
#define QD_DEEP_DISORDERED_PROP

#include "proportional.hpp"

namespace sferes {
  namespace qd {
    namespace selector {      

      //Selector proportional for disordered deep grid
      //Return one of the element of one of the filled disordered cell with fitness-proportional probability
      DEEP_PROPORTIONATE(Deep_disordered_proportional)
      {
       public:

	typedef boost::shared_ptr<Phen> indiv_t;
	typedef typename std::vector<indiv_t> pop_t;
	typedef boost::shared_ptr<Cell> cell_t;
	

	// Select a pop from the whole grid
	template <typename EA>
	void operator()(pop_t& pop, const EA& ea) const
	{
	  std::vector<cell_t> filled_content;
	  ea.container().get_filled_content(filled_content);
	  for (auto& indiv : pop) 
	    {
	      // Choose a random cell uniformely
	      cell_t cell = filled_content[misc::rand<int>(0, filled_content.size())];
	      
	      // Choose a random indiv proportionnaly to the fitness inside it
	      size_t size = cell->size();
	      if (size > 1)
	      {
		 std::tuple<float, std::vector<float>> fitness = this->relative_fitness(cell);
	         indiv = cell->operator[](this->select_prop(size, std::get<1>(fitness), std::get<0>(fitness)));
	      }
	      else
	          indiv = cell->operator[](0);
	    }
	}
	

	// Return best individual of the cell
	void operator()(indiv_t& indiv, const cell_t cell) const
	{ indiv = cell->operator[](this->best_indiv(cell)); }
	

	// Select a pop from one cell
	void operator()(pop_t& pop, const cell_t cell) const
	{
	  int size = cell->size();
	  std::tuple<float, std::vector<float>> fitness = this->relative_fitness(cell);
	  for (auto& indiv : pop)
	      indiv = cell->operator[](this->select_prop(size, std::get<1>(fitness), std::get<0>(fitness)));
	}
	
      };
    } // namespace selector
  } // namespace qd
} // namespace sferes
#endif
