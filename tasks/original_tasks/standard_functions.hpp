#ifndef ___AIRL_TASKS_STANDARD_FUNCTIONS_HPP__
#define ___AIRL_TASKS_STANDARD_FUNCTIONS_HPP__

#include <iostream>
#include <Eigen/Core>

#include <sferes/gen/evo_float.hpp>
#include <sferes/phen/parameters.hpp>
#include <sferes/fit/fit_qd.hpp>

using namespace sferes::gen::evo_float;

namespace standard_functions {
  // Definition of default parameter values
  struct Params_default {
    struct parameters {
      SFERES_CONST float min = -5.12;
      SFERES_CONST float max =  5.12;
    };
    
    struct evo_float {
      SFERES_CONST float cross_rate = 0.75f; // cross over helps a lot in these experiments
      SFERES_CONST float mutation_rate = 0.05f;
      SFERES_CONST float eta_m = 10.0f;
      SFERES_CONST float eta_c = 10.0f;
      SFERES_CONST mutation_t mutation_type = polynomial;
      SFERES_CONST cross_over_t cross_over_type = sbx;
    };
    struct qd {
      SFERES_CONST size_t behav_dim = 2;
      SFERES_ARRAY(size_t, grid_shape, 100, 100);
    };
    
    struct task{
      enum Function {sphere, rastrigin, rastrigin_multi};
      SFERES_CONST Function function = sphere;
      SFERES_CONST size_t gen_dim = 100;
    };

    struct nov {
      SFERES_CONST size_t deep = 2;
      SFERES_CONST double l = 0.01; 
      SFERES_CONST double k = 25; 
      SFERES_CONST double eps = 0.1;
    };
  

  };
  

  
  // Definition of the fitness function
  FIT_QD(Standard_functions){
    
  public :
     template<typename Indiv>
      void eval(Indiv& ind)
    {

      std::vector<double> bd;
      std::tie(this->_value, bd, this->_dead) = simulate(ind.data());
      this->set_desc(bd);

    }


    // simulate function that receives the controller parameters and outputs the fitness and DB 
    std::tuple<float, std::vector<double>, bool > simulate (const std::vector<float>& ctrl) const
    {

      float value = 0;
      if(Params::task::function == Params::task::sphere)
	value = 0;
      else // RASTRIGIN and RASTRIGIN_MULTI
	value =  - 10.0 * (double) ctrl.size();
      
      std::vector<double> vec;
      for (size_t i = 0; i < ctrl.size(); ++i)
	{
	  float xi=(ctrl[i] - 5.12 * 0.4); //ofset as defined in the paper
	  if(Params::task::function == Params::task::sphere)
	    value +=  - xi*xi;
	  else // RASTRIGIN and RASTRIGIN_MULTI
	    value +=  - ( xi*xi  - 10.0*cos(2*M_PI * xi));
	  
	  vec.push_back(ctrl[i]);
	}
      std::vector<double> desc = p(vec);

      return {value, desc, false};
    }    
    
    std::vector<double> p(std::vector<double> vec) const
    {
      double p1=0;
      double p2=0;
      if(Params::task::function == Params::task::rastrigin_multi){
	p1 += _clip(vec[0])/5.12+0.5;
	p2 += _clip(vec[1])/5.12+0.5;
      }
      else { // SPHERE and RASTRIGIN
	for (size_t i = 0; i < floor(vec.size()/2); ++i)
	  p1+=_clip(vec[i]);
	for (size_t i = floor(vec.size()/2); i < vec.size(); ++i)
	  p2+=_clip(vec[i]);
	p1=p1/((double)vec.size()*5.12)+0.5;
	p2=p2/((double)vec.size()*5.12)+0.5;
      }
      
      p1 = std::max(0.0,std::min(1.0,p1));
      p2 = std::max(0.0,std::min(1.0,p2));
      return {p1,p2};
      
    }
    
    double _clip(double x) const
    {
      if(-5.12 <= x && x <=5.12)
	return x;
      else
	return 5.12/x;
    }
    
  };
  
  // Definition of the Genotype, Phenotype and Fitness object according to a provided Param Object.
  template<typename Params>
  using fit_t = Standard_functions<Params>;
  template<typename Params>
  using gen_t = sferes::gen::EvoFloat<Params::task::gen_dim, Params>;
  template<typename Params>
  using phen_t = sferes::phen::Parameters<gen_t<Params>, fit_t<Params>, Params>;
  
}

#endif
