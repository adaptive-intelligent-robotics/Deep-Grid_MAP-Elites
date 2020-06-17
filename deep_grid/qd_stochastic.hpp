#ifndef QD_STOCHASTIC_HPP_
#define QD_STOCHASTIC_HPP_

#include <sferes/qd/quality_diversity.hpp>

namespace sferes {
  namespace qd {
    
    template <typename Phen, typename Eval, typename Stat, typename FitModifier, 
	      typename Selector, typename Container, typename Cell, typename Params, typename Exact = stc::Itself>
      class QD_Stochastic : public qd::QualityDiversity<Phen, Eval, Stat, FitModifier, Selector, Container, Params, 
							typename stc::FindExact< QD_Stochastic<Phen, Eval, Stat, 
							FitModifier, Selector, Container, Cell, Params, Exact>, 
							Exact>::ret>
    {
    public:
      
      typedef boost::shared_ptr<Phen> indiv_t;
      typedef typename std::vector<indiv_t> pop_t;
      typedef boost::shared_ptr<Cell> cell_t;
      
      // Select a sample population from a given cell
      pop_t sample(const size_t n, const cell_t cell, const bool best_only = false) const
      {
	if (best_only)
	{
	  indiv_t indiv;
	  this->_selector(indiv, cell);  
	  pop_t sample(n, indiv);
	  return sample;
	}
	pop_t sample(n);
	this->_selector(sample, cell);
	return sample;
      }
      pop_t select_best_cell(const size_t n, const std::vector<double> desc, const bool best_only = false) const 
      { return sample(n, this->_container.get_cell(desc), best_only); }

    };    
  } // namespace qd
} // namespace sferes
#endif

