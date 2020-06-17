#include "matplotlibcpp.h"
namespace plt = matplotlibcpp;
#include "utils.cpp"
#include "graph.cpp"
#include "archive.cpp"

// Given the information of one line of a task file, open it and plot/store datas
void open_datas(std::string dir, std::string subdir, std::string name, std::string task,
		std::vector<size_t> grid_dims, double radius, bool plot_archives,
		std::vector<std::string> stat_files, std::vector<std::string> stat_types,
		std::vector<double>& gens, std::vector<double>& evals, 
		std::vector<std::tuple<std::string, std::vector<double>>>& datas)
{
  std::cout<<std::endl;
  std::cout<<"Considering "<<name<<std::endl;
  
  // For each of the stat file
  assert (stat_files.size() == stat_types.size());
  for (size_t file = 0; file < stat_files.size(); file++)
    {
      // If graph type, save the datas
      if (stat_types[file]=="graph")
	read_graph(subdir, stat_files[file], datas, gens, evals);
      
      // If archive type, plot it or write its path
      if (stat_types[file]=="archive" && plot_archives)
	{
          // Name and min and max value of the archive
	  std::string name_archive = task + "_" + name + "_-_" 
		  + stat_files[file].substr(0,stat_files[file].find("-"));

	  // If it is a polar archive (if the radius is non-zero), add its path to the archives.dat file
	  if (radius > 0)
	    {
	      // Open file
	      std::cout<<"Writting the path of archive in "<<stat_files[file]<<std::endl;
	      std::ofstream archive_file;
	      archive_file.open(dir + "/archives.dat", std::ios_base::app);

	      // Write the name of the archive, the path to it, grid dim, radius and min-max
	      std::string write = name_archive + "," + subdir + "/" + stat_files[file] + "," 
		      + std::to_string(grid_dims[0]) + "," + std::to_string(radius) + ","
		      + std::to_string(0.0) + "," + std::to_string(1.0);
	      archive_file<<write<<std::endl;
	      archive_file.close();
	    }
	  
	  // Else plot the archive
	  else
	    {
	      // Read the archive and plot it
	      std::vector<std::vector<double>> descs;
	      std::vector<double> fitness;
	      read_archive(subdir, stat_files[file], grid_dims, descs, fitness);
	      plot_archive(descs, fitness, name_archive, dir, grid_dims, 0.0, 1.0);
	    }
	}
    }
  std::cout<<std::endl;
}

// Find all tasks files open them, call other functions to analyse and plot
void analyse_data(std::string dir)
{
  // Go trhough all subdirs to find all tasks files
  std::vector<std::string> list_files = find_task_files(dir);
  
  // Create data containers
  std::vector<std::vector<std::vector<std::tuple<std::string, std::vector<double>>>>> datas;
  std::vector<std::vector<std::vector<double>>> gens, evals;
  std::vector<std::string> tasks_names;
  std::vector<std::vector<std::string>> names;

  // Open archive file and write the header in it
  std::ofstream archive_file;
  archive_file.open(dir + "/archives.dat");
  archive_file<<"name,path,dim,radius,min,max"<<std::endl;
  archive_file.close();
  
  // Open all task files one by one
  for (std::string task_file : list_files)
    {
      std::ifstream file;
      file.open(task_file);
      
      // For each line of it
      size_t n_line = 0;
      for (std::string line; std::getline(file, line);)
	{
	      // Read the info on this experiment
	      std::string subdir, name, task;
	      std::vector<size_t> grid_dims;
	      double radius = 0;
	      std::vector<std::string> stat_files, stat_types;
	      read_line(task_file, line, subdir, name, task, grid_dims, radius, stat_files, stat_types);

	      // Determine the corresponding task index
	      size_t task_index = std::distance(tasks_names.begin(), 
			      std::find(tasks_names.begin(), tasks_names.end(), task));
	      if (task_index >= tasks_names.size())
	      {
		tasks_names.push_back(task);
		names.resize(tasks_names.size());
		datas.resize(tasks_names.size());
		gens.resize(tasks_names.size());
		evals.resize(tasks_names.size());
	      }
	     
	      // Create local containers for datas 
	      std::vector<std::tuple<std::string, std::vector<double>>> local_datas;
	      std::vector<double> local_gens, local_evals;
	      bool plot_archives = //if an archive has already been plotted for this variant and this task
		         std::find(names[task_index].begin(), names[task_index].end(), name) 
			 == names[task_index].end(); 
	      
	      // Open the data
	      open_datas(dir, subdir, name, task, grid_dims, radius, plot_archives,
			 stat_files, stat_types, local_gens, local_evals, local_datas);
	      
	      // Fill in global containers
	      names[task_index].push_back(name);
	      datas[task_index].push_back(local_datas);
	      gens[task_index].push_back(local_gens);
	      evals[task_index].push_back(local_evals);
	  n_line++;
	}
    }

  write_csv(tasks_names, gens, evals, datas, names, dir);
}

