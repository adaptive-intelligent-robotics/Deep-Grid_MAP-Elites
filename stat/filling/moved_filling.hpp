namespace sferes {
  namespace stat {
    
    // Filling of the moved grid in cells
    SFERES_STAT(Filling_cell_moved, Stat) {
    public:
      
      template<typename E> void refresh(const E& ea) 
      {
	if (ea.gen() % Params::pop::dump_period == 0)
	  {
	    _moved_size = boost::fusion::at_c<PHANTOM_MODIFIER>(ea.fit_modifier()).filled_moved_cells();
	    _moved_relative_size = (double)_moved_size /(double) ea.container().get_number_cells()*100;
	    
	    if (ea.dump_enabled())
	      {
		this->_create_log_file(ea, _file_name);
		(*this->_log_file) 	<< ea.gen() 
					<< " Eval:" << ea.container().total_eval()
					<< " filling_Moved_collection_size_(cells):" << _moved_size
					<< " filling_Relative_moved_collection_size_(%_of_cells):" << _moved_relative_size
					<< std::endl;
	      }
	  }
      }
      
      const std::string file_name() const { return _file_name; }
      const std::string type() const { return _type; }

    protected:
      double _moved_size = 0;
      double _moved_relative_size = 0;
      std::string _file_name = "filling_moved.dat";
      std::string _type = "graph";
    };
  } //stat
} //sferes
