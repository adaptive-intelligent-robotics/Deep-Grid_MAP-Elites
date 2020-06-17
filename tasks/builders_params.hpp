// File use to define all Params common to all stochastic tasks
struct Params : Params_default {

  struct pop { 
    SFERES_CONST int init_size = 1000;
    SFERES_CONST int size = 100;
    static int nb_gen; 
    static int nb_eval; 
    static int use_eval; 
    static int dump_period;
    static int dump_ini;
    SFERES_CONST int adaptive_sample_dump_factor = 5;
    SFERES_CONST int no_sample_dump_factor = 8;
  };
  
  struct evo_float { 
    static float cross_rate; 
    static float mutation_rate;
    static float deep_mutation_rate; //mutation rate for deep algorithms
    SFERES_CONST float eta_m = 10.0f;
    SFERES_CONST float eta_c = 10.0f;
    SFERES_CONST mutation_t mutation_type = polynomial;
    SFERES_CONST cross_over_t cross_over_type = sbx;
  };
  
  struct stochastic{
    static int deep_size; //depth of the grid, - can be set for any algorithm
    static int N_sampling; //number of sampling replications for naive sampling
    static std::vector<int> deep_depth; //vector of multiple depth for deep-grid - run them in the same execution
    static std::vector<int> adapt_depth; //similar for adaptive with drifting elites
    static std::vector<int> N_values; //similar for naive but with number of sample instead of depth
    SFERES_CONST int N_repets = 50; //number of repetition to evaluate the ground truth of an individual
    SFERES_CONST int N_best_repets = 50; //number of repetition to evaluate the ground truth of best individual
  }; 
  
  struct noise{
    static float fit_var; //gaussian fitness noise variance
    static float desc_var; //gaussian bd noise variance
  };		

  struct algos{
    static bool Truth; //Noise-free baseline
    static bool Naive; //Naive sampling algorithm
    static bool Adaptive; //Adaptive sampling without drifting elites
    static bool Adaptive_BD; //Adaptive sampling with drifting elites
    static bool Deep; //Deep grid
  };

  struct archive{ // Radius of the polar archive
#ifndef POLAR_COORD //non polar archive: no radius
    SFERES_CONST double radius = 0.0;
#else //polar archive
    SFERES_CONST double radius = 0.5;
#endif
  };
};

int Params::pop::nb_gen = 50001;
int Params::pop::nb_eval = 100000;
int Params::pop::dump_period = 500;
int Params::pop::dump_ini = 500;
int Params::pop::use_eval = false;

float Params::evo_float::cross_rate = 0.0f;
float Params::evo_float::mutation_rate = 0.1f;
float Params::evo_float::deep_mutation_rate = 0.05f;

int Params::stochastic::deep_size = 1;
int Params::stochastic::N_sampling = 1;
std::vector<int> Params::stochastic::deep_depth = {50};
std::vector<int> Params::stochastic::adapt_depth = {10, 50};
std::vector<int> Params::stochastic::N_values = {1, 50};

float Params::noise::fit_var = 0.1;
float Params::noise::desc_var = 0.01;

bool Params::algos::Truth = false;
bool Params::algos::Naive = false;
bool Params::algos::Adaptive = false;
bool Params::algos::Adaptive_BD = false;
bool Params::algos::Deep = false;

