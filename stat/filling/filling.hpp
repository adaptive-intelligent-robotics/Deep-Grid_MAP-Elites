namespace sferes {
  namespace stat {

    // Filling of the grid in individuals
    SFERES_STAT(Filling_indiv, Stat) {
    public:
      
      template<typename E> void refresh(const E& ea) 
      {
	if (ea.gen() % Params::pop::dump_period == 0)
	  {
	    auto filled_content = ea.container().get_filled_content();
	    _size = ea.container().total_indiv();
	    _relative_size = (double)_size / 
		    (double)(ea.container().get_number_cells()*Params::stochastic::deep_size) * 100;
	    _mean_depth = 0;
	    for(auto cell : filled_content)
	      _mean_depth += cell->size();
	    _mean_depth /= (double) filled_content.size();
	    _relative_mean_depth = _mean_depth / (double) Params::stochastic::deep_size * 100;
	    
	    if (ea.dump_enabled())
	      {
		this->_create_log_file(ea, _file_name);
		(*this->_log_file) 	<< ea.gen() 
					<< " Eval:" << ea.container().total_eval()
					<< " filling_Collection_size_(indiv):" << _size
					<< " filling_Relative_collection_size_(%_indiv):" << _relative_size
					<< " filling_Mean_depth_(indiv):" << _mean_depth 
					<< " filling_Relative_mean_depth_(%_indiv):" << _relative_mean_depth 
					<< std::endl;
	      }
	  }
      }
      
      const double collection_size() const { return _size;}
      const double mean_depth() const { return _mean_depth;}
      
      const std::string file_name() const { return _file_name; }
      const std::string type() const { return _type; }

    protected:
      double _size = 0;
      double _relative_size = 0;
      double _mean_depth = 0;
      size_t _relative_mean_depth = 0;
      std::string _file_name = "filling_indiv.dat";
      std::string _type = "graph";
    };
    
    // Filling of the grid in cells
    SFERES_STAT(Filling_cell, Stat) {
    public:
      
      template<typename E> void refresh(const E& ea) 
      {
	if (ea.gen() % Params::pop::dump_period == 0)
	  {
	    auto filled_content = ea.container().get_filled_content();
	    _size = filled_content.size();
	    _relative_mean_depth = 0;
	    for(auto cell : filled_content)
	      _relative_mean_depth += cell->size();
	    _relative_mean_depth = _relative_mean_depth 
		    / ((double) filled_content.size() * (double) Params::stochastic::deep_size) * 100;
	    
	    if (ea.dump_enabled())
	      {
		this->_create_log_file(ea, _file_name);
		(*this->_log_file) 	<< ea.gen() 
					<< " Eval:" << ea.container().total_eval()
					<< " filling_Collection_size_(cells):" << _size
					<< " filling_Mean_depth_(%):" << _relative_mean_depth 
					<< std::endl;
	      }
	  }
      }
      
      const std::string file_name() const { return _file_name; }
      const std::string type() const { return _type; }

    protected:
      double _size = 0;
      size_t _relative_mean_depth = 0;
      std::string _file_name = "filling_cells.dat";
      std::string _type = "graph";
    };
  } //stat
} //sferes
