#include <sferes/run.hpp>
#include <sferes/misc.hpp>

// Struct to write datas on stats
struct file_name_f {
  template<typename T>
  void operator() (T & x) const 
  {
    name += x.file_name() + ",";
    type += x.type() + ",";
  }
  mutable std::string name;
  mutable std::string type;
};


// Write task file 
template<typename Params>
void write_task(std::string subdir, std::string algo, std::string stats, std::string types)
{
  // Open file
  boost::shared_ptr<std::ofstream> tasks_file = boost::shared_ptr<std::ofstream>(
		new std::ofstream(Params::stats::dir+"/tasks.dat", std::fstream::app));
  
  // Fill in grid dimensions
  std::string grid_dims = "";
  for (size_t i = 0; i < Params::qd::grid_shape_size(); ++i)
    grid_dims += std::to_string(Params::qd::grid_shape(i))+",";
  
  // Write file
  if (tasks_file->is_open())
    {
      *tasks_file 	<< "Dir:" << subdir
			<< " Task:" << Params::stats::task_name
			<< " Algo:" << algo
			<< " BD:" << grid_dims
			<< " Radius:" << Params::archive::radius
			<< " N_gens:" << std::to_string(Params::pop::nb_gen)
			<< " N_evals:" << std::to_string(Params::pop::nb_eval)
			<< " Mutation_rate:" << Params::evo_float::mutation_rate
			<< " Stats:" << stats
			<< " Types:" << types
			<< std::endl;
    }
  else
    std::cout << "Error opening Task file"<<std::endl;
  std::cout<<std::endl;
};


// Run baseline
template<typename phen_t, typename eval_t, typename stat_t, typename modifier_t, typename Params>
void run_baseline(std::string exp_name)
{
  using namespace sferes;
  Params::stochastic::deep_size = 1;
  Params::pop::dump_period = Params::pop::dump_ini * Params::pop::no_sample_dump_factor;

  std::cout<<std::endl;
  std::cout<<"Running baseline: "<<exp_name<<std::endl;

  typedef qd::MAP<phen_t, eval_t, stat_t, modifier_t, Params> truth_t;
  truth_t truth;
  truth.set_res_dir(Params::stats::dir+"/"+exp_name);
  truth.run();
  
  stat_t stat_list = truth.stat();
  file_name_f stats_files; 
  boost::fusion::for_each(stat_list, stats_files);
  write_task<Params>(truth.res_dir(), exp_name, stats_files.name, stats_files.type);
};


// Run all experiments
template<typename phen_t, typename eval_t, typename stat_t, typename modifier_t, typename Params>
void run_experiments()
{
  using namespace sferes;

  // Naive 
  if (Params::algos::Naive)
    {
      Params::stochastic::deep_size = 1;

      for (size_t j = 0; j < Params::stochastic::N_values.size(); ++j) // For each number of sample value
	{
	  Params::stochastic::N_sampling = Params::stochastic::N_values[j];
	  Params::pop::dump_period = Params::stochastic::N_sampling > 10 ? Params::pop::dump_ini :
                  Params::pop::dump_ini * Params::pop::no_sample_dump_factor;

          std::cout<<std::endl;
          std::cout<<"Running naive sampling algorithm with "<<Params::stochastic::N_sampling;
	  std::cout<<" samples"<<std::endl;
	  std::string exp_name = std::to_string(Params::stochastic::N_sampling)+"_smpl_Naive";

	  typedef qd::Naive_sampling<phen_t, eval_t, stat_t, modifier_t, Params> naive_t;
	  naive_t naive;
	  naive.set_res_dir(Params::stats::dir+"/"+exp_name);
	  naive.run();
	  
	  stat_t stat_list = naive.stat();
	  file_name_f stats_files; 
	  boost::fusion::for_each(stat_list, stats_files);
	  write_task<Params>(naive.res_dir(), exp_name, stats_files.name, stats_files.type);
	}
    }
  
  // Adaptive without drifting elites
  if (Params::algos::Adaptive)
    {
      Params::stochastic::deep_size  = 1;
      Params::pop::dump_period = Params::pop::dump_ini * Params::pop::adaptive_sample_dump_factor;

      std::cout<<std::endl;
      std::cout<<"Running adaptive sampling algorithm"<<std::endl;
      std::string exp_name = "Adaptive_sampling";

      typedef qd::Adaptive_MAP<phen_t, eval_t, stat_t, modifier_t, Params> adapt_t;
      adapt_t adapt;
      adapt.set_res_dir(Params::stats::dir+"/"+exp_name);
      adapt.run();
      
      stat_t stat_list = adapt.stat();
      file_name_f stats_files; 
      boost::fusion::for_each(stat_list, stats_files);
      write_task<Params>(adapt.res_dir(), exp_name, stats_files.name, stats_files.type);
    }
  
  // Adaptive with drifting elites 
  for (size_t i = 0; i < Params::stochastic::adapt_depth.size(); ++i) // For each value of depth
    {
      if (Params::algos::Adaptive_BD)
	{
	  Params::stochastic::deep_size = Params::stochastic::adapt_depth[i];
	  Params::pop::dump_period = Params::pop::dump_ini * Params::pop::adaptive_sample_dump_factor;

	  std::cout<<std::endl;
	  std::cout<<"Running adaptive sampling with behavior correction algorithm and depth "
		  <<std::to_string(Params::stochastic::deep_size)<<std::endl;
	  std::string exp_name = "Adaptive_BD_sampling_" + std::to_string(Params::stochastic::deep_size);

	  typedef qd::Adaptive_BD_MAP<phen_t, eval_t, stat_t, modifier_t, Params> adapt_bd_t;
	  adapt_bd_t adapt_bd;
	  adapt_bd.set_res_dir(Params::stats::dir+"/"+exp_name);
	  adapt_bd.run();
	  
	  stat_t stat_list = adapt_bd.stat();
	  file_name_f stats_files; 
	  boost::fusion::for_each(stat_list, stats_files);
	  write_task<Params>(adapt_bd.res_dir(), exp_name, stats_files.name, stats_files.type);
	}
    }
  
  
  // Deep grid
  for (size_t i = 0; i < Params::stochastic::deep_depth.size(); ++i) // For each value of depth
    {
      Params::evo_float::mutation_rate = Params::evo_float::deep_mutation_rate;

      if (Params::algos::Deep)
	{
          Params::stochastic::deep_size = Params::stochastic::deep_depth[i];
	  Params::pop::dump_period = Params::pop::dump_ini * Params::pop::no_sample_dump_factor;

  	  std::cout<<std::endl;
    	  std::cout<<"Running deep algorithm with deep "<<Params::stochastic::deep_size<<std::endl;
	  std::string exp_name = "Deep_grid_" + std::to_string(Params::stochastic::deep_size);

	  typedef qd::Deep_grid<phen_t, eval_t, stat_t, modifier_t, Params> deep_t;
	  deep_t deep;
	  deep.set_res_dir(Params::stats::dir+"/"+exp_name);
	  deep.run();

	  stat_t stat_list = deep.stat();
	  file_name_f stats_files; 
	  boost::fusion::for_each(stat_list, stats_files);
	  write_task<Params>(deep.res_dir(), exp_name, stats_files.name, stats_files.type);
	}
    }
};


