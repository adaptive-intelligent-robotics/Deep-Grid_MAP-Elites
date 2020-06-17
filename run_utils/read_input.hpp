#include <boost/math/special_functions/erf.hpp>

// Function to analyse inputs
template<typename Params>
void read_input(int argc, char **argv)
{
  std::vector<std::string> v;
  for (size_t i=1; i<argc; i++)
    v.push_back(argv[i]);
  
  // experiments choice
  auto it = std::find(v.begin(), v.end(), "--all");
  if(it != v.end())
  {
      Params::algos::Truth = true;
      Params::algos::Naive = true;
      Params::algos::Adaptive = true;
      Params::algos::Adaptive_BD = true;
      Params::algos::Deep = true;
  }
  it = std::find(v.begin(), v.end(), "--truth");
  if(it != v.end())
      Params::algos::Truth = true;
  it = std::find(v.begin(), v.end(), "--naive");
  if(it != v.end())
      Params::algos::Naive = true;
  it = std::find(v.begin(), v.end(), "--adapt");
  if(it != v.end())
      Params::algos::Adaptive = true;
  it = std::find(v.begin(), v.end(), "--adapt_BD");
  if(it != v.end())
      Params::algos::Adaptive_BD = true;
  it = std::find(v.begin(), v.end(), "--deep");
  if(it != v.end())
      Params::algos::Deep = true;

  // directory
  it = std::find(v.begin(), v.end(), "-d");
  if(it != v.end())
    Params::stats::dir = *(it+1);
  
  // use eval or gen
  it = std::find(v.begin(), v.end(), "-e");
  if(it != v.end())
    Params::pop::use_eval = true;
  
  // number of evals
  it = std::find(v.begin(), v.end(), "-n");
  if(it != v.end())
    {
      if (Params::pop::use_eval)
	Params::pop::nb_eval = std::stoi(*(it+1));
      else
	Params::pop::nb_gen = std::stoi(*(it+1));
    }
  
  // dump period
  it = std::find(v.begin(), v.end(), "-p");
  if(it != v.end())
    Params::pop::dump_ini = std::stoi(*(it+1));
  
  // noise variance parameters
  it = std::find(v.begin(), v.end(), "--fit_var");
  if(it != v.end())
    Params::noise::fit_var = std::stod(*(it+1));
  it = std::find(v.begin(), v.end(), "--desc_var");
  if(it != v.end())
    Params::noise::desc_var = std::stod(*(it+1));

  // depth and number of sampling
  it = std::find(v.begin(), v.end(), "--n_sampling");
  if(it != v.end())
      Params::stochastic::N_values = {std::stoi(*(it+1))};
  it = std::find(v.begin(), v.end(), "--deep_depth");
  if(it != v.end())
      Params::stochastic::deep_depth = {std::stoi(*(it+1))};
  it = std::find(v.begin(), v.end(), "--adapt_depth");
  if(it != v.end())
      Params::stochastic::adapt_depth = {std::stoi(*(it+1))};
  
  // mutation
  it = std::find(v.begin(), v.end(), "--mut");
  if(it != v.end())
    Params::evo_float::mutation_rate = std::stod(*(it+1));
  it = std::find(v.begin(), v.end(), "--cross");
  if(it != v.end())
    Params::evo_float::cross_rate = std::stod(*(it+1));
  it = std::find(v.begin(), v.end(), "--deep_mut");
  if(it != v.end())
    Params::evo_float::deep_mutation_rate = std::stod(*(it+1));
};

// Print task informations
template<typename Params>
void print_task()
{
  // task
  std::cout<<std::endl;
  std::cout<<"Running "<<Params::stats::task_name;
  std::cout<<" and saving results in: "<<Params::stats::dir<<std::endl;
  
  // Number gens/evals
  std::string number = Params::pop::use_eval? std::to_string(Params::pop::nb_eval)+" evals" 
	  : std::to_string(Params::pop::nb_gen)+" gens";
  std::cout<<"	-> "<<number;
  std::cout<<", with initial dump period: ";
  std::cout<<Params::pop::dump_ini<<" gens."<<std::endl;
  
  // Noise parameters
  std::cout<<"	-> noise on fitness variance: "<<Params::noise::fit_var;
  std::cout<<", and on desc: "<<Params::noise::desc_var<<std::endl;
};

