////////////////////////////////// Read file function //////////////////////////////

void read_graph(std::string subdir, std::string stat_file,
		std::vector<std::tuple<std::string, std::vector<double>>>& datas, 
		std::vector<double>& gens, std::vector<double>& evals)
{
  std::cout<<"Reading the graph in "<<stat_file<<std::endl;

  // Open the file
  std::ifstream open_file;
  open_file.open(subdir+"/"+stat_file);
  
  // For each line of it
  for (std::string line; std::getline(open_file, line);)
    {
      // Split the line with the different stats, for each stat
      std::vector<std::string> stats = split(line, ' ');
      for (std::string stat : stats)
	{
	  // Split between name and value
	  std::vector<std::string> values = split(stat, ':');
	  
	  // if the considered stat is the generation number
	  if (values.size()==1)
	    {
	      double value = std::stod(values[0]);
	      // if new gen number
	      if (gens.empty() || std::find(gens.begin(), gens.end(), value) == gens.end())
		gens.push_back(value);
	    }
	  
	  // if it is the number of evals
	  else if (values[0] == "Eval")
	    {
	      double value = std::stod(values[1]);
	      if (gens.empty() || std::find(evals.begin(), evals.end(), value) == evals.end())
		evals.push_back(value);
	    }
	  
	  // if it is any other stat
	  else if (values.size()>1)
	    {
	      // value and where to add it
	      double value = std::stod(values[1]);
	      size_t max_index = datas.size();
	      size_t index = max_index;
	      
	      // Look for the name of this stat
	      for (size_t i = 0; i < max_index; i++)
		if (std::get<0>(datas[i]) == values[0])
		  index = i;
	      
	      // If not already in datas
	      if (index == max_index)
		{
		  std::tuple<std::string, std::vector<double>> tuple = {values[0], {value}};
		  datas.push_back(tuple);
		}
	      // If already there, add it
	      else
		std::get<1>(datas[index]).push_back(value);
	    }
	}
    }
}

////////////////////////////////// Save data as csv for seaborn function //////////////////////////////

void write_csv(std::vector<std::vector<double>> gens, std::vector<std::vector<double>> evals, 
	       std::vector<std::vector<std::tuple<std::string, std::vector<double>>>> datas, 
	       std::vector<std::string> names, std::string dir, std::string task)
{
  std::cout<<"Start writting csv files"<<std::endl;
  
  // Open a txt file to write datas information for gitlab
  std::ofstream stat_file;
  stat_file.open (dir + "/stats_" + task + ".dat");

  // Find all similar algorithms
  assert ( datas.size() == names.size() );
  std::vector<std::vector<size_t>> similars;
  std::vector<std::string> real_names;
  find_similar_algos(names, similars, real_names);
  const size_t N_algos = real_names.size();
  stat_file<<N_algos<<" algos"<<std::endl;
  std::cout<<"Number of algos: "<<N_algos<<std::endl;
  
  // Find all graphs
  size_t N_graphs = 0;
  for (size_t index = 0; index < datas.size(); index++)
    if (datas[index].size() > N_graphs)
      N_graphs = datas[index].size();
  std::cout<<"Number of graphs for eval and for gen: "<<N_graphs<<std::endl;

  // Number of csv excluding others
  const size_t N_combinations = combinations_files.size();
  const size_t N_prefixes = prefixes.size();
  const size_t N_comb_csv = N_prefixes * N_combinations; 

  // Find graphs indexes and write column names
  std::vector<std::string> graphs_names;
  std::vector<std::string> name_col (N_prefixes);
  std::vector<bool> write_file (N_prefixes);
  std::string other_col;
  std::vector<std::vector<size_t>> list_indexes;
  for (size_t graph = 0; graph < N_graphs; graph++)
    {
      std::vector<size_t> indexs;
      find_indexes(datas, graphs_names, indexs, graph);
      list_indexes.push_back(indexs);
      
      size_t prefixe = 0;
      while (prefixe < N_prefixes 
	     && graphs_names[graph].find(prefixes[prefixe]) == std::string::npos)
	prefixe++;
      if (prefixe == N_prefixes)
 	other_col += "," + graphs_names[graph]; 
      else
	{
	  name_col[prefixe] += "," + graphs_names[graph];
	  write_file[prefixe] = true;
	}
    }
  bool write_others = !other_col.empty();
  
  // Open all basic files and write column names
  std::ofstream eval_files[N_prefixes];
  std::ofstream gen_files[N_prefixes];
  std::ofstream eval_other_file;
  std::ofstream gen_other_file;
  if (write_others)
    {
      eval_other_file.open (dir + "/" + task + "_other_eval.csv");
      eval_other_file<<"eval,variant"<<other_col<<std::endl;
      gen_other_file.open (dir + "/" + task + "_other_gen.csv");
      gen_other_file<<"gen,variant"<<other_col<<std::endl;
    }
  for (size_t prefixe = 0; prefixe < N_prefixes; prefixe++)
    if (write_file[prefixe])
      {
	eval_files[prefixe].open(dir + "/" + task + "_" + prefixes[prefixe] + "_eval.csv");
	eval_files[prefixe]<<"eval,variant"<<name_col[prefixe]<<std::endl;
	gen_files[prefixe].open(dir + "/" + task + "_" + prefixes[prefixe] + "_gen.csv");
	gen_files[prefixe]<<"gen,variant"<<name_col[prefixe]<<std::endl;
      }
  
  // Open all combination files
  std::ofstream eval_comb_files[N_comb_csv];
  std::ofstream eval_comb_other_file[N_combinations];
  for (size_t comb = 0; comb < N_combinations; comb++)
    {
      // Open other files
      if (write_others)
	{
	  eval_comb_other_file[comb].open (dir + "/" + task + "_other_" + combinations_names[comb] + ".csv");
	  eval_comb_other_file[comb]<<"eval,variant"<<other_col<<std::endl;
	}
      // Open all prefixe files
      for (size_t prefixe = 0; prefixe < N_prefixes; prefixe++)
	if (write_file[prefixe])
	  {
		  std::cout<<prefixes[prefixe]<<std::endl;
	    eval_comb_files[comb*N_prefixes + prefixe]
		    .open(dir + "/" + task + "_" + prefixes[prefixe] + "_" + combinations_names[comb] + ".csv");
	    eval_comb_files[comb*N_prefixes +prefixe]<<"eval,variant"<<name_col[prefixe]<<std::endl;
	  }
    }
  
  // For each of the algo
  for (size_t algo = 0; algo < N_algos; algo++)
    {
      size_t N = similars[algo].size();
      size_t N_real = N; //real number of algorithms, excluding the ones which did not run
      size_t N_lines = gens[similars[algo][0]].size(); //number of line for this algo
      std::string name_algo = real_names[algo];
      
      // For each algorithm variant
      for (size_t variant = 0; variant < N; variant++)
	{
	  size_t index_variant = similars[algo][variant];
	  bool run = true; //check if this algorithm actually run
	  
	  // For each line
	  for (size_t line = 0; line < N_lines; line++)
	    {
	      size_t N_non_value = 0; //check if this  algo has run at least one of the stat
	      std::vector<std::string> prefixe_line (N_prefixes);
	      std::string other_line;
	      
	      // For each column / each graph
	      for (size_t graph = 0; graph < N_graphs; graph++) // Each graph is a column
		{
		  
		  // Read the graph index and deduce the value
		  size_t index_graph = list_indexes[graph][index_variant];
		  float value = 0;
		  if (index_graph != 0)
		    {
		      value = std::get<1>(datas[index_variant][index_graph-1])[line];
		      
		      // If the value is NaN or too big to be a real value
		      size_t current_variant = 0;
		      size_t previous_line = line-1;
		      size_t next_line = line+1;
		      while (value != value || value > 1000000000)
			{
			  // First try to use the other variant to find a plausible value
			  if (current_variant < N)
			    {
			      if (std::get<1>(datas[similars[algo][current_variant]][index_graph-1]).size()>line)
			         value = std::get<1>(datas[similars[algo][current_variant]][index_graph-1])[line];
			      current_variant++;
			    }
			  // If all of them have infinite values, try to use previous and following line
			  else if (previous_line > 0 && (line - previous_line <= next_line - line))
			    {
		              value = std::get<1>(datas[index_variant][index_graph-1])[previous_line];
		              previous_line--;
			    }
			  else if (next_line < N_lines)
			    {
		              value = std::get<1>(datas[index_variant][index_graph-1])[next_line];
		              next_line++;
			    }
			  // If none of this work, put value 0
			  else
			    value = 0;
			}
		    }
		  else
		    N_non_value++;
		  
		  
		  // Find in which file it should be written
		  size_t prefixe = 0;
		  while (prefixe < N_prefixes 
			 && graphs_names[graph].find(prefixes[prefixe]) == std::string::npos)
		    prefixe++;
		  if (prefixe == N_prefixes)
		    other_line += "," + std::to_string(value); 
		  else
		    prefixe_line[prefixe] += "," + std::to_string(value);
		} //end graph loop
	      
	      // if the variant has at least one of the stat, print it
	      if (N_non_value < N_graphs)
		{
		  std::string nb_eval = std::to_string(evals[similars[algo][0]][line]) + ",";
		  std::string nb_gen = std::to_string(gens[similars[algo][0]][line]) + ",";
		  
		  // Basic files	      
		  if (write_others)
		    {
		      eval_other_file<< nb_eval << name_algo << other_line <<std::endl;
		      gen_other_file<< nb_gen << name_algo << other_line <<std::endl;
		    }
		  for (size_t prefixe = 0; prefixe < N_prefixes; prefixe++)
		    if (write_file[prefixe])
		      {
			eval_files[prefixe]<< nb_eval << name_algo << prefixe_line[prefixe] <<std::endl;
			gen_files[prefixe]<< nb_gen << name_algo << prefixe_line[prefixe]<<std::endl;
		      }
		  
		  // Combination files
		  for (size_t comb = 0; comb < N_combinations; comb++)
		    {
		      for (std::string combinations_algo : combinations_files[comb])
			if ( name_algo.find(combinations_algo) != std::string::npos)
			  {
			    // Open other files
			    if (write_others)
			      eval_comb_other_file[comb]<< nb_eval << name_algo << other_line <<std::endl;
			    // Open all prefixe files
			    for (size_t prefixe = 0; prefixe < N_prefixes; prefixe++)
			      if (write_file[prefixe])
				eval_comb_files[comb*N_prefixes + prefixe]
					<< nb_eval << name_algo << prefixe_line[prefixe] <<std::endl;
			  }
		    }
		  
		}
	      // else, indicate this is a false run
	      else
		run = false;
	    } // end line loop
	  
	  // If this is a false run, substract one of the real run
	  if (!run)
	    N_real--;
	} //end variant loop
      
      // Write the stat file
      stat_file<<N_real<<" ";
      stat_file<<name_algo<<std::endl;
    } //end algo loop
  
  // Close all files
  stat_file.close();
  eval_other_file.close();
  gen_other_file.close();
  for (size_t prefixe = 0; prefixe < N_prefixes; prefixe++)
    {
      eval_files[prefixe].close();
      gen_files[prefixe].close();
    }
  for (size_t comb = 0; comb < N_combinations; comb++)
    {
      eval_comb_other_file[comb].close();
      for (size_t prefixe = 0; prefixe < N_prefixes; prefixe++)
	eval_comb_files[comb*N_prefixes + prefixe].close();
    }
  std::cout<<"End writting csv files"<<std::endl;
}

// Cal write csv for each task
void write_csv( std::vector<std::string> tasks_names,
		std::vector<std::vector<std::vector<double>>> gens, 
		std::vector<std::vector<std::vector<double>>> evals, 
		std::vector<std::vector<std::vector<std::tuple<std::string, std::vector<double>>>>> datas, 
		std::vector<std::vector<std::string>> names, std::string dir )
{
  std::ofstream tasks_file;
  tasks_file.open (dir + "/tasks_overview.dat");
  for (size_t task = 0; task < tasks_names.size(); task++)
  {
    write_csv(gens[task], evals[task], datas[task], names[task], dir, tasks_names[task]);
    tasks_file<<tasks_names[task]<<" ";
    tasks_file<<"stats_" + tasks_names[task] + ".dat"<<std::endl;
  }
  tasks_file.close();
}
