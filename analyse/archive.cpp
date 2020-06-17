////////////////////////////////// Read file function //////////////////////////////

// Read data in an archive file
void read_archive(std::string subdir, std::string stat_file, std::vector<size_t> grid_dims,
		std::vector<std::vector<double>>& descs, std::vector<double>& fitness)
{
  std::cout<<"Plotting the archive in "<<stat_file;

  // Open the file
  std::ifstream open_file;
  open_file.open(subdir+"/"+stat_file);
  
  // For each line of it
  size_t N_line = 0;
  for (std::string line; std::getline(open_file, line);)
    {
      // Split the line with the different stats, for each stat
      std::vector<std::string> stats = split(line, ' ');
      
      // First value is useless and the dim following one are the descriptor values
      std::vector<double> desc;
      for (size_t i = 1; i < grid_dims.size()+1; i++)
	desc.push_back(std::stod(stats[i]));
      descs.push_back(desc);
      
      // The following one is the fitness
      fitness.push_back(std::stod(stats[grid_dims.size()+1]));
      N_line++;
    }
  std::cout<<", number of line: "<<N_line<<std::endl;
}

// Return point size depending on the grid dimensions
double get_point_size(std::vector<size_t> grid_dims){
  double val1=1;
  for(size_t i=0; i<grid_dims.size();i+=2)
    val1*=grid_dims[i];
  double val2=1;
  for(size_t i=1; i<grid_dims.size();i+=2)
    val2*=grid_dims[i];
  double val=std::max(val1,val2);
  return 100000.0/(val*val);
}

////////////////////////////////// Save data as csv for seaborn function //////////////////////////////

// Plot the archive
void plot_archive(std::vector<std::vector<double>> descs, std::vector<double> fitness, 
		std::string name, std::string dir, std::vector<size_t> grid_dims, 
		double min_archive, double max_archive)
{
  double size = get_point_size(grid_dims);
  size = size*70/100;
  
  // Generalisation of the plot for multiple dimensions
  std::vector<double> x,y;
  for ( std::vector<double> desc : descs )
    {
      double valx = 0;
      double valy = 0;
      double space = 1;
      for(size_t i=0; i<grid_dims.size();i+=2)
	{
	  valx  += std::round(desc[i]*(grid_dims[i]-1))*(space);
	  space += grid_dims[i];
	}
      space = 1;
      for(size_t i=1; i<grid_dims.size();i+=2)
	{
	  valy  += std::round(desc[i]*(grid_dims[i]-1))*(space);
	  space += grid_dims[i];
	}
      x.push_back(valx);
      y.push_back(valy);
    }
  // Compute axes limit
  size_t extrem1 = 0;
  double space = 1;
  for(size_t i=0; i<grid_dims.size();i+=2)
  {
	  extrem1 += (grid_dims[i]-1) * space;
	  space += grid_dims[i];
  }
  size_t extrem2 = 0;
  space = 1;
  for(size_t i=1; i<grid_dims.size();i+=2)
  {
	  extrem2 += (grid_dims[i]-1) * space;
	  space += grid_dims[i];
  }
  
  // Add two extrem points 
  fitness.push_back(max_archive);
  x.push_back(0.95*extrem1);
  y.push_back(0.95*extrem2);
  fitness.push_back(min_archive);
  x.push_back(0.05*extrem1);
  y.push_back(0.05*extrem2);

  // Plot
  std::string name_archive = name;
  std::replace( name_archive.begin(), name_archive.end(), '_', ' ');
  plt::figure();
  plt::scatter(x, y, fitness, size, true);
  plt::title(name_archive);
  plt::xlim(-1, (int) (extrem1 + 1));
  plt::ylim(-1, (int) (extrem2 + 1));
  plt::axis("off");
  plt::save(dir+"/"+name+".png");
  plt::close();
}
