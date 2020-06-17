#include <iostream>
#include <boost/multi_array.hpp>
#include <boost/filesystem.hpp>
#include <cmath>
#include <sferes/run.hpp>
#include <fstream>

// Define colors
std::vector<std::string> colors = 
	{"blue", "orange", "green", "red", 
		"purple", "brown", "pink", 
		"grey", "olive", "cyan"};

// Define combination of algorithm that will define a graph plot with only these algos
std::vector<std::vector<std::string>> combinations_files = { 
	//{"Baseline", "Deep_grid_", "Adaptive_BD_sampling_"}
	};
std::vector<std::string> combinations_names = {
	//"simple"
	};

// Define all stats types prefixes that will be save in different csv
std::vector<std::string> prefixes = 
  { "cell_fitness", "cell_stability", "best_fitness", "best_stability", "filling"};


// Main
#include "analyse/analyse.hpp"

int main(int argc, char **argv)
{
  std::string dir = "/git/sferes2/build/exp/rastrigin_stochastic";
  if (argc>1)
    {
      std::vector<std::string> v;
      for (size_t i=1; i<argc; i++)
	v.push_back(argv[i]);
      // Looking for directory
      auto it_dir = std::find(v.begin(), v.end(), "-d");
      if(it_dir != v.end())
	dir = *(it_dir+1);
    }
  std::cout << "Starting analysis in "<<dir<<std::endl;
  analyse_data(dir);
  std::cout << "End analysis" <<std::endl;
}


      
