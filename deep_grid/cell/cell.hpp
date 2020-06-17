#include "default_polar.hpp"

namespace sferes {
  namespace qd {
    namespace container {

      // Basic cell
      template <typename Phen, typename Params> class Cell : public Cell_default<Phen, Params> {
	
       protected:
	typedef boost::shared_ptr<Phen> indiv_t;

	// Get descriptor of a given indiv
	std::vector<double> _desc(const indiv_t& indiv) const override { return indiv->fit().desc(); }

      };


      // Cell returning the first desc instead of the mean
      template <typename Phen, typename Params> class Cell_First_desc : public Cell_default<Phen, Params> {
	
       protected:
	typedef boost::shared_ptr<Phen> indiv_t;

	// Get descriptor of a given indiv
	std::vector<double> _desc(const indiv_t& indiv) const { return indiv->fit().first_desc(); }

      };

    } //container
  } //qd
} //sferes

