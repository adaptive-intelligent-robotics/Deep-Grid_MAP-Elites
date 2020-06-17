#include <sferes/stc.hpp>
#include <boost/multi_array.hpp>

#include <sferes/modif/dummy.hpp>
#include "eval_count.hpp"
#include "phantom.hpp"

// Use to call the corresponding modifier to write the stats
#define PHANTOM_MODIFIER 0
#define PHANTOM_BEST_MODIFIER 1

namespace sferes
{
  namespace modifier_list
  {
    template<typename Params>
    using modifier_t = boost::fusion::vector<
      modif::Phantom<Params>, //Compute the stat on each cell of the grid
#ifdef BEST
      modif::Phantom_best<Params>, //Compute the stat on each best_indiv of the grid
#endif
      modif::Eval_count<Params> //Change number of gen to match number of eval
      >;
    
  };
};
