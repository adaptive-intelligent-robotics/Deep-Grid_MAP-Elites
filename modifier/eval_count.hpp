#ifndef MODIFIER_EVAL_COUNT_HPP
#define MODIFIER_EVAL_COUNT_HPP

namespace sferes {
  namespace modif {
    template <typename Params, typename Exact = stc::Itself>
    class Eval_count {
    public:
      template<typename Ea>
      void apply(Ea& ea) 
      {
	// If still not enough eval, add a dump period
	if (Params::pop::use_eval 
	    && ea.gen() > Params::pop::nb_gen-2 
	    && ea.container().total_eval()<Params::pop::nb_eval)
	  Params::pop::nb_gen = Params::pop::nb_gen + Params::pop::dump_period;

        // If enought eval, remove a dump period
	if (Params::pop::use_eval 
	    && ea.container().total_eval() > Params::pop::nb_eval
	    && Params::pop::nb_gen >
	    ((ea.gen() / Params::pop::dump_period + 1) * Params::pop::dump_period + 1))
	  Params::pop::nb_gen = (ea.gen() / Params::pop::dump_period + 1) * Params::pop::dump_period + 1;
      }
    };
  }
}

#endif
