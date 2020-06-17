#ifndef ___AIRL_TASKS_HEXAPOD_OMNIDIRECTIONAL_HPP__
#define ___AIRL_TASKS_HEXAPOD_OMNIDIRECTIONAL_HPP__

#include <sferes/gen/evo_float.hpp>
#include <sferes/phen/parameters.hpp>
#include <sferes/fit/fit_qd.hpp>

#include <robot_dart/robot_dart_simu.hpp>
#include <robot_dart/control/hexa_control.hpp>

#ifdef GRAPHIC
#include <robot_dart/gui/magnum/graphics.hpp>
#endif


#include <dart/collision/bullet/BulletCollisionDetector.hpp>
#include <dart/constraint/ConstraintSolver.hpp>

#include "desc_hexa.hpp"




using namespace sferes::gen::evo_float;

namespace hexapod_omnidirectional {
  
  namespace global{
    std::shared_ptr<robot_dart::Robot> global_robot;
  }
  
  // Definition of default parameter values
  struct Params_default {
    struct parameters {
      SFERES_CONST float min = 0;
      SFERES_CONST float max = 1;
    };
    struct evo_float {
      SFERES_CONST float cross_rate = 0.0f;
      SFERES_CONST float mutation_rate = 0.03f;
      SFERES_CONST float eta_m = 10.0f;
      SFERES_CONST float eta_c = 10.0f;
      SFERES_CONST mutation_t mutation_type = polynomial;
      SFERES_CONST cross_over_t cross_over_type = sbx;
    };

    struct nov {
        SFERES_CONST size_t deep = 3;
        SFERES_CONST double l = 0.01; 
        SFERES_CONST double k = 15; 
        SFERES_CONST double eps = 0.1;
    };

    struct qd {
      SFERES_CONST size_t behav_dim = 2;
      SFERES_ARRAY(size_t, grid_shape, 100,100);
    };

  };
  
  




  // b-a (function for angle calculation)
  double angle_dist(double a, double b)
  {
    double theta = b - a;
    while (theta < -M_PI)
      theta += 2 * M_PI;
    while (theta > M_PI)
      theta -= 2 * M_PI;
    return theta;
  }
  
  
  void load_and_init_robot(std::string path)
  {
    std::cout<<"INIT Robot"<<std::endl;
    global::global_robot = std::make_shared<robot_dart::Robot>(path+"/hexapod_v2.urdf");
    std::cout<<"INIT Robot"<<std::endl;
    global::global_robot->set_position_enforced(true);
    //global::global_robot->set_position_enforced(true);
    //global_robot->skeleton()->setPosition(1,100* M_PI / 2.0);
    
    global::global_robot->set_actuator_types(dart::dynamics::Joint::SERVO);
    global::global_robot->skeleton()->enableSelfCollisionCheck();
    std::cout<<"End init Robot"<<std::endl;
  }
  
  FIT_QD(Hexapod_omnidirectional)
  {
  public:
    Hexapod_omnidirectional(){  }
    
    typedef Eigen::Matrix<float,Eigen::Dynamic,Eigen::Dynamic, Eigen::RowMajor > Mat;
    
    
    template<typename Indiv>
      void eval(Indiv& ind)
    {

      std::vector<double> bd;
      std::tie(this->_value, bd, this->_dead) = simulate(ind.data());
      this->set_desc(bd);

    }
    


    // simulate function that receives the controller parameters and outputs the fitness, DB and potential death
    std::tuple<float, std::vector<double>, bool > simulate (const std::vector<float>& indiv) const
    {
      std::vector<double> ctrl(36);
      for(size_t i=0;i<indiv.size();i++)
	{
	  ctrl[i] = round( std::min(1.0f, std::max(0.0f,indiv[i])) * 1000.0 ) / 1000.0;// limite numerical issues
	}
      
      auto g_robot=global::global_robot->clone();
      g_robot->skeleton()->setPosition(5, 0.15);
      
      
      double ctrl_dt = 0.02;
      g_robot->add_controller(std::make_shared<robot_dart::control::HexaControl>(ctrl_dt, ctrl));
      std::static_pointer_cast<robot_dart::control::HexaControl>(g_robot->controllers()[0])->set_h_params(std::vector<double>(1, ctrl_dt));
      
      robot_dart::RobotDARTSimu simu(0.02);
#ifdef GRAPHIC
      auto graphics = std::make_shared<robot_dart::gui::magnum::Graphics<>>(simu.world());
      simu.set_graphics(graphics);
#endif
      
      simu.world()->getConstraintSolver()->setCollisionDetector(dart::collision::BulletCollisionDetector::create());
      simu.add_floor();
      simu.add_robot(g_robot);
      
      simu.add_descriptor(std::make_shared<robot_dart::descriptor::TrajDescriptor>(robot_dart::descriptor::TrajDescriptor(simu)));
      
      Eigen::VectorXd init_trans = simu.robots().back()->skeleton()->getPositions();
      
      simu.run(3);

      //obtain required data, fitness, descriptors
      // for final position of the robot. not need for this as descriptor is already the x and y position already b
      std::vector<Eigen::VectorXf> pos_results; // results is the final position of the robot (used to get the descriptor)
      std::static_pointer_cast<robot_dart::descriptor::TrajDescriptor>(simu.descriptor(0))->get(pos_results); //getting descriptor

      double x, y ; 
      x = (pos_results.back()(0)) ; // only want the final x and y position only so  .back. (0) is the x coordinate  
      y = (pos_results.back()(1)) ; //(1) is the y coordinate 

      // scaling (scaling based on estimate of travel distance etc) to maximise and ensure repertoire between 0 and 1. can also try to increase archive size to 100x100
      std::vector<double> desc = {(x+1.5)/3, (y+1.5)/3};
      
      Eigen::VectorXd pose = simu.robots().back()->skeleton()->getPositions();
      double error = orientation_error(pose, init_trans);
      
      g_robot.reset();

      bool dead = false;
      if(desc[0]<0 || desc[0]>1 ||desc[1]<0 || desc[1]>1)
	dead=true; //if something is wrong, we kill this solution. 
      

      
      return { - error, desc, dead };
    }
    
    double orientation_error(const Eigen::VectorXd& pose, const Eigen::VectorXd& init_trans) const
    {
      Eigen::Vector3d final_pos;
      Eigen::Vector3d final_rot;
      double arrival_angle;
      double covered_distance;
      
      Eigen::Matrix3d rot = dart::math::expMapRot({pose[0], pose[1], pose[2]});
      Eigen::Matrix3d init_rot = dart::math::expMapRot({init_trans[0], init_trans[1], init_trans[2]});
      Eigen::MatrixXd init_homogeneous(4, 4);
      init_homogeneous << init_rot(0, 0), init_rot(0, 1), init_rot(0, 2), init_trans[3], init_rot(1, 0), init_rot(1, 1), init_rot(1, 2), init_trans[4], init_rot(2, 0), init_rot(2, 1), init_rot(2, 2), init_trans[5], 0, 0, 0, 1;
      Eigen::MatrixXd final_homogeneous(4, 4);
      final_homogeneous << rot(0, 0), rot(0, 1), rot(0, 2), pose[3], rot(1, 0), rot(1, 1), rot(1, 2), pose[4], rot(2, 0), rot(2, 1), rot(2, 2), pose[5], 0, 0, 0, 1;
      Eigen::Vector4d pos = {init_trans[3], init_trans[4], init_trans[5], 1.0};
      pos = init_homogeneous.inverse() * final_homogeneous * pos;
      
      final_pos = pos.head(3);
      
      covered_distance = std::round(final_pos(0) * 100) / 100.0;
      
      // Angle computation
      final_rot = dart::math::matrixToEulerXYZ(init_rot.inverse() * rot);
      
      // roll-pitch-yaw
      arrival_angle = std::round(final_rot(2) * 100) / 100.0;

      
      // Performance - Angle Difference (desrird angle and obtained angle fomr simulation)
      // Change of orientation of axis in counting the desried angle to account for frontal axis of the newer robot (x-axis:frontal axis)
      double x = final_pos[0] ;
      double y = final_pos[1] ;
      
      // Computation of desired angle (yaxis-north x-axis(postive))
      double B = std::sqrt((0.25 * x * x) + ( 0.25 * y * y));
      double alpha = std::atan2(y, x);
      double A = B / std::cos(alpha);
      double beta = std::atan2(y, x - A);
      
      if (x < 0)
	beta = beta - M_PI;
      while (beta < -M_PI)
	beta += 2 * M_PI;
      while (beta > M_PI)
	beta -= 2 * M_PI;
      
      double angle_diff = std::abs(angle_dist(beta, arrival_angle)); //angle dist was a finction made earlier up in the script
      
      return angle_diff;
    }
    
    
    
    
    
  private:

    std::vector<Eigen::VectorXf> _traj;
    
    
  };
  
  // Definition of the Genotype, Phenotype and Fitness object according to a provided Param Object.
  template<typename Params>
  using fit_t = Hexapod_omnidirectional<Params>;
  template<typename Params>
  using gen_t = sferes::gen::EvoFloat<36, Params>;
  template<typename Params>
  using phen_t = sferes::phen::Parameters<gen_t<Params>, fit_t<Params>, Params>;

  
}

#endif
