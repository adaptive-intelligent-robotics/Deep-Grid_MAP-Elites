namespace sferes {
  namespace stat {
    // Total stability of the archive
    SFERES_STAT(Total_stability, Stat) {
    public: 
      
      template<typename E> void refresh(const E& ea)
      {
	if (ea.gen() % Params::pop::dump_period == 0)
	  {
	    _relative_correct_desc = boost::fusion::at_c<PHANTOM_MODIFIER>
		    (ea.fit_modifier()).prop_static_cells() * 100;
	    _correct_desc = boost::fusion::at_c<PHANTOM_MODIFIER>(ea.fit_modifier()).static_cells();
	    _mean_jump = boost::fusion::at_c<PHANTOM_MODIFIER>(ea.fit_modifier()).mean_cell_move();
	    _mean_move = boost::fusion::at_c<PHANTOM_MODIFIER>(ea.fit_modifier()).mean_move();
	    
	    if (ea.dump_enabled())
	      {
		this->_create_log_file(ea, _file_name);
		(*this->_log_file) 	<< ea.gen() 
					<< " Eval:" << ea.container().total_eval()
					<< " cell_stability_Proportion_correct_descriptor_(%):" << _relative_correct_desc
					<< " cell_stability_Number_correct_descriptor:" << _correct_desc
					<< " cell_stability_Mean_cell_jump:" << _mean_jump
					<< " cell_stability_Mean_descriptor_move:" << _mean_move
					<<std::endl;
	      }
	  }
      }
      
      const std::string file_name() const { return _file_name; }
      const std::string type() const { return _type; }
      
    protected:
      double _correct_desc = 0;
      double _relative_correct_desc = 0;
      double _mean_jump = 0;
      double _mean_move = 0;
      std::string _file_name = "stability.dat";
      std::string _type = "graph";
    };
  } //stat
} //sferes
