#ifndef QD_DEEP_UNIFORM
#define QD_DEEP_UNIFORM

namespace sferes {
  namespace qd {
    namespace selector {

      //Selector uniform for deep grid
      //Return first element of one of the filled cell of the grid
      template <typename Phen, typename Cell, typename Params> struct Deep_uniform {
	
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
	      int cell_num = misc::rand<int>(0, filled_content.size());
	      indiv = filled_content[cell_num]->operator[](0);
	    }
	}

	// Return best individual of the cell
	void operator()(indiv_t& indiv, const cell_t cell) const
	{ indiv = cell->operator[](0); };

	// Select a pop from one cell
	void operator()(pop_t& pop, const cell_t cell) const
	{
	  for (auto& indiv : pop)
	      indiv = cell->operator[](0);
	}
	
      };
    } // namespace selector
  } // namespace qd
} // namespace sferes
#endif
