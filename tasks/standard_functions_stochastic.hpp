#ifndef __FIT_STOCHASTIC_HPP__
#define __FIT_STOCHASTIC_HPP__

#include <boost/multi_array.hpp>
#include "original_tasks/standard_functions.hpp"

// General namespace for stochastic fitness
namespace fit_stochastic 
{
  typedef std::vector<double> desc_t;
  
  // Parameters namespace 
  namespace params_stochastic 
  {
    struct Params_default : standard_functions::Params_default {
      
      struct parameters {
	SFERES_CONST float min = -5.12;
	SFERES_CONST float max =  5.12;
      };

      struct stats{
	static std::string dir;
	static std::string task_name;
	SFERES_CONST float min = -450;
	SFERES_CONST float max = 0;
      };

      struct task{
	enum Function {sphere, rastrigin, rastrigin_multi};
	SFERES_CONST Function function = rastrigin_multi;
	SFERES_CONST size_t gen_dim = 6;
      };

      struct qd {
	SFERES_CONST size_t behav_dim = 2;
	SFERES_ARRAY(size_t, grid_shape, 100, 100);
      };
    };
    std::string Params_default::stats::dir = "/git/sferes2/build/exp/deep_grid";
    std::string Params_default::stats::task_name = "Rastrigin";
    #include "builders_params.hpp"
  };
  

  // Task parameters
  namespace task
  {  
    // Define arm stochastic functions
    template <typename Params> class Fit_stochastic_base: 
      public standard_functions::fit_t<Params>
    {
     protected:
      
      // No preprocessing nor death in this case
      void _preprocess(const std::vector<float>& ctrl) {};
      bool _is_dead(float value, desc_t desc) const { return false; };
      
      // Gaussian noise on desc and fit
      void _apply_noise_desc(desc_t& desc, const float value) const
      {
	for (size_t i = 0; i < desc.size(); ++i)
	  desc[i] = sferes::misc::put_in_range(desc[i] + 
			  sferes::misc::gaussian_rand(0.0, (double) Params::noise::desc_var), 0.0, 1.0);
      };
      void _apply_noise_value(float& value) const
      { value = value + sferes::misc::gaussian_rand(0.0, (double) Params::noise::fit_var); };
    };
    
    // Name use by main 
    #include "builders_fit.hpp"
    void load_and_init_robot() {};
    void reset_robot() {};
    template<typename Params> using fit_t = Fit_stochastic<Params>; 
    template<typename Params> using gen_t = standard_functions::gen_t<Params>;
    template<typename Params> using phen_t = sferes::phen::Parameters<gen_t<Params>, fit_t<Params>, Params>;
  };

  // Max baseline namespace
  namespace baseline_max
  {
    // Define arm stochastic functions
    template <typename Params> class Fit_stochastic_base: 
      public standard_functions::fit_t<Params>
    {
     protected:
      // No preprocessing nor death in this case
      void _preprocess(const std::vector<float>& ctrl) {};
      bool _is_dead(float value, desc_t desc) const { return false; };
      
      // No noise in this case
      void _apply_noise_desc(desc_t& desc, const float value) const {};
      void _apply_noise_value(float& value) const {};
    };
    
    // Name use by main 
    #include "builders_fit.hpp"
    template<typename Params> using fit_t = Fit_stochastic<Params>; 
    template<typename Params> using gen_t = standard_functions::gen_t<Params>;
    template<typename Params> using phen_t = sferes::phen::Parameters<gen_t<Params>, fit_t<Params>, Params>;
  };
};

#endif

