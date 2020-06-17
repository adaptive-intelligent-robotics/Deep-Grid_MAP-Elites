#ifndef __FIT_STOCHASTIC_HPP__
#define __FIT_STOCHASTIC_HPP__

#include <boost/multi_array.hpp>
#include "original_tasks/redundant_arm.hpp"

// General namespace for stochastic fitness
namespace fit_stochastic 
{
  typedef std::vector<double> desc_t;
  
  // Parameters namespace 
  namespace params_stochastic 
  {
    struct Params_default : redundant_arm::Params_default {

      struct stats{
	static std::string dir;
	static std::string task_name;
	SFERES_CONST float min = -3.5;
	SFERES_CONST float max = 0;
      };

#ifndef POLAR_COORD 
      struct qd {
	SFERES_CONST size_t behav_dim = 2;
	SFERES_ARRAY(size_t, grid_shape, 100, 100);
      };
#else
      struct qd {
	SFERES_CONST size_t behav_dim = 2;
	SFERES_ARRAY(size_t, grid_shape, 350, 350);
      };
#endif
      
    };
    std::string Params_default::stats::dir = "/git/sferes2/build/exp/deep_grid";
    std::string Params_default::stats::task_name = "Arm_var";
    #include "builders_params.hpp"
  };
  

  // Task parameters
  namespace task
  {  
    // Define arm stochastic functions
    template <typename Params> class Fit_stochastic_base: 
      public redundant_arm::fit_t<Params>
    {
     protected:
      
      // No preprocessing in this case
      void _preprocess(const std::vector<float>& ctrl) {};

      // Test if an individual is outside of the circle or has an abnormal value
      bool _is_dead(float value, desc_t desc) const
      { return (value < -100); };
      
      // Method to apply noise on desc using the value
      void _apply_noise_desc(desc_t& desc, const float value) const
      {
	for (size_t i=0; i < desc.size(); i++)
	  desc[i] = sferes::misc::put_in_range(desc[i] 
	 	+ sferes::misc::gaussian_rand(0.0, (double)Params::noise::desc_var), 0.0, 1.0);
      };
      
      // Method to apply noise on value
      void _apply_noise_value(float& value) const
      { value = value + sferes::misc::gaussian_rand(0.0, (double) Params::noise::fit_var); };
    };
    
    // Name use by main 
    #include "builders_fit.hpp"
    void load_and_init_robot() {};
    void reset_robot() {};
    template<typename Params> using fit_t = Fit_stochastic<Params>; 
    template<typename Params> using gen_t = redundant_arm::gen_t<Params>;
    template<typename Params> using phen_t = sferes::phen::Parameters<gen_t<Params>, fit_t<Params>, Params>;
  };
  

  // Max baseline namespace
  namespace baseline_max
  {
    // Define arm stochastic functions
    template <typename Params> class Fit_stochastic_base: 
      public redundant_arm::fit_t<Params>
    {
     protected:
      // No preprocessing in this case
      void _preprocess(const std::vector<float>& ctrl) {};

      // Test if an individual is outside of the circle or has an abnormal value
      bool _is_dead(float value, desc_t desc) const { return (value < -100); };
      
      // No noise on either desc or value in this case
      void _apply_noise_desc(desc_t& desc, const float value) const {};
      void _apply_noise_value(float& value) const {};
    };
    
    // Name use by main 
    #include "builders_fit.hpp"
    template<typename Params> using fit_t = Fit_stochastic<Params>; 
    template<typename Params> using gen_t = redundant_arm::gen_t<Params>;
    template<typename Params> using phen_t = sferes::phen::Parameters<gen_t<Params>, fit_t<Params>, Params>;
  };
};

#endif

