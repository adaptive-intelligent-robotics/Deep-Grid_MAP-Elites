// Split a string according to a delimitor delim
std::vector<std::string> split(std::string& str, const char& delim)
{
  std::vector<std::string> result;
  std::string next;
  for (std::string::const_iterator it = str.begin(); it != str.end(); it++)
    {
      if (*it == delim)
        {
          if (!next.empty())
            {
              result.push_back(next);
              next.clear();
            }
        }
      else
        next += *it;
    }
  if (!next.empty())
    result.push_back(next);
  return result;
}



// Recursively go through all subdirs to collect all task files
std::vector<std::string> find_task_files(std::string dir)
{
  std::vector<std::string> listOfFiles;

  // if dir is really a directory and already exist
  if (boost::filesystem::exists(dir) && boost::filesystem::is_directory(dir))
    {
      boost::filesystem::recursive_directory_iterator iter(dir);
      boost::filesystem::recursive_directory_iterator end;

      // Iterate among all files
      while (iter != end)
	{

	  // If directory, recursively go throught it
	  if (boost::filesystem::is_directory(iter->path()))
	    {
	      std::vector<std::string> subfiles = find_task_files(iter->path().string());
	      for (std::string file : subfiles)
		listOfFiles.push_back(file);
	      iter.no_push();
	    }

	  // Else, look for tasks.dat files
	  else
	    {
	      std::string name = iter->path().string();
	      if (name.find("tasks.dat") != std::string::npos)
		{
		  std::cout<<"Find tasks.dat in "<<name<<std::endl;
		  listOfFiles.push_back(name);
		}
	    }
	  boost::system::error_code ec;
	  iter.increment(ec);
	}
    }
  return listOfFiles;
}


// Read a line of a task file and fill in the informations about it
void read_line(std::string file, std::string line, 
	       std::string& subdir, std::string& name, std::string& task,
	       std::vector<size_t>& grid_dims, double& radius,
	       std::vector<std::string>& stat_files, std::vector<std::string>& stat_types)
{
  // For each entry of this line
  std::vector<std::string> texts = split( line, ' ');
  for (std::string text : texts)
    {
      std::vector<std::string> part = split( text, ':');
      
      // If contain the dimensions of BD
      if (part[0]=="BD")
	{
	  std::vector<std::string> files_test = split( part[1], ',');
	  for (size_t i=0; i<files_test.size(); i++)
	    grid_dims.push_back(std::stoi(files_test[i]));
	}
      
      // If contain the radius
      if (part[0]=="Radius")
	 radius = std::stod(part[1]);
      
      // If contain the type of experiment, convert to subdir name
      if (part[0]=="Algo")
	{
          std::string dir (file.begin(), file.begin() + file.find("tasks.dat")); //get current path
	  subdir = dir + part[1];
	  name = part[1];
	}
      
      // If contain the stats files
      if (part[0]=="Stats")
	stat_files = split( part[1], ',');
      
      // If contain the type of each stat
      if (part[0]=="Types")
	stat_types = split( part[1], ',');
      
      // If contain the task
      if (part[0]=="Task")
	task = part[1];
    }
}


// Find in the name list which algo are replications of the same
void find_similar_algos(std::vector<std::string> names, std::vector<std::vector<size_t>>& similars,
		std::vector<std::string>& real_names)
{
  // For all algos in names
  for (size_t i = 0; i<names.size(); i++)
    {
      auto it = std::find(real_names.begin(), real_names.end(), names[i]);
      // If this algo appears for the first time
      if (it  == real_names.end())
	{
	  real_names.push_back(names[i]);
	  std::vector<size_t> vector;
	  vector.push_back(i);
	  similars.push_back(vector);
	}
      // else, add its position to the corresponding list
      else
	  similars[std::distance(real_names.begin(), it)].push_back(i);
    }
}

// Find the indexes of the given graph for each of the algo replication
void find_indexes(std::vector<std::vector<std::tuple<std::string, std::vector<double>>>> datas,
		  std::vector<std::string>& graphs_names, std::vector<size_t>& indexs, size_t graph)
{
  // If does not already know the name of this graph, find the first non-plotted graph
  if (graphs_names.size() <= graph)
    {
      size_t algo = 0;
      while (graphs_names.size() <= graph && algo < datas.size())
	{
	  size_t index_graph = 0;
	  while (graphs_names.size() <= graph && index_graph < datas[algo].size())
	    {
	      if (std::find(graphs_names.begin(), graphs_names.end(),
			    std::get<0>(datas[algo][index_graph])) == graphs_names.end())
		  graphs_names.push_back(std::get<0>(datas[algo][index_graph]));
	      index_graph++;
	    }
	  algo++;
	}
    }
  
  // For all algos runs
  indexs.clear();
  for (size_t algo = 0; algo < datas.size(); algo++)
    {
      // Find the index corresponding to this graph
      if (indexs.size() <= algo)
	for (size_t i = 0; i < datas[algo].size(); i++)
	  if (std::get<0>(datas[algo][i]) == graphs_names[graph])
	    indexs.push_back(i+1);  //use i+1 to signale inexisting graph with 0
      
      // If not found then put a 0
      if (indexs.size() <= algo)
	indexs.push_back(0);
    }
}

