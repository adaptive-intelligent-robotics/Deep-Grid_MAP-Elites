#include <iostream>
#include <Eigen/Core>

#include <sferes/gen/evo_float.hpp>
#include <sferes/phen/parameters.hpp>
#include <sferes/fit/fit_qd.hpp>

using namespace sferes::gen::evo_float;

namespace redundant_arm {
  // Definition of default parameter values
  struct Params_default {
    
    struct parameters {
      SFERES_CONST float min = -M_PI;
      SFERES_CONST float max = M_PI;
    };
    struct evo_float {
      SFERES_CONST float cross_rate = 0.0f;
      SFERES_CONST float mutation_rate = 0.15f;
      SFERES_CONST float eta_m = 10.0f;
      SFERES_CONST float eta_c = 10.0f;
      SFERES_CONST mutation_t mutation_type = polynomial;
      SFERES_CONST cross_over_t cross_over_type = sbx;
    };

    struct nov {
      SFERES_CONST size_t deep = 2;
      SFERES_CONST double l = 0.01; 
      SFERES_CONST double k = 25; 
      SFERES_CONST double eps = 0.1;
    };
  

    struct qd {
      SFERES_CONST size_t behav_dim = 2;
      SFERES_ARRAY(size_t, grid_shape, 100, 100);
    };

  };
  
  

  // Definition of the fitness function
  FIT_QD(Redundant_arm){
     
  public : 
    template <typename Indiv> 
      void eval(Indiv & ind)
    {

      std::vector<double> bd;
      std::tie(this->_value, bd, this->_dead) = simulate(ind.data());
      this->set_desc(bd);

    }



    // simulate function that receives the controller parameters and outputs the fitness and DB 
    std::tuple<float, std::vector<double>, bool > simulate (const std::vector<float>& ctrl) const
    {
      size_t nb_dofs= ctrl.size();
      Eigen::VectorXd angle(nb_dofs);
      for (size_t i = 0; i < nb_dofs; ++i)
	angle[i] = ctrl[i];

      // fitness:
      float value = - sqrt((angle.array()-angle.mean()).square().mean());

      // Behavioral descriptor:
      Eigen::Vector3d pos = forward_model(angle);
      std::vector<double> data = {pos[0]/2 + 0.5, pos[1]/2 + 0.5};

      return {value, data, false};
    }

    
    
    Eigen::Vector3d forward_model(const Eigen::VectorXd a)const {
      
      Eigen::VectorXd _l_arm=Eigen::VectorXd::Ones(a.size()+1);
      _l_arm(0)=0;
      _l_arm = _l_arm/_l_arm.sum();
      
      Eigen::Matrix4d mat=Eigen::Matrix4d::Identity(4,4);
      for(size_t i=0;i<a.size();i++){
	Eigen::Matrix4d submat;
	submat<<cos(a(i)), -sin(a(i)), 0, _l_arm(i), sin(a(i)), cos(a(i)), 0 , 0, 0, 0, 1, 0, 0, 0, 0, 1;
	mat=mat*submat;
      }
      
      Eigen::Matrix4d submat;
      submat<<1, 0, 0, _l_arm(a.size()), 0, 1, 0 , 0, 0, 0, 1, 0, 0, 0, 0, 1;
      mat=mat*submat;
      Eigen::VectorXd v=mat*Eigen::Vector4d(0,0,0,1);
      
      return v.head(3);
      
    }
    
  };

  // Definition of the Genotype, Phenotype and Fitness object according to a provided Param Object.
  template<typename Params>
  using fit_t = Redundant_arm<Params>;
  template<typename Params>
  using gen_t = sferes::gen::EvoFloat<8, Params>;
  template<typename Params>
  using phen_t = sferes::phen::Parameters<gen_t<Params>, fit_t<Params>, Params>;
  
}

