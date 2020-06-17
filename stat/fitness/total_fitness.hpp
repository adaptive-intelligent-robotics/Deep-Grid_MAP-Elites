namespace sferes {
  namespace stat {
    // Total quality of the archive
    SFERES_STAT(Total_quality, Stat) {
    public: 
      
      template<typename E> void refresh(const E& ea)
      {
	if (ea.gen() % Params::pop::dump_period == 0)
	  {
	    _quality = boost::fusion::at_c<PHANTOM_MODIFIER>(ea.fit_modifier()).original_quality();
	    _moved_quality = boost::fusion::at_c<PHANTOM_MODIFIER>(ea.fit_modifier()).moved_quality();
	    
	    if (ea.dump_enabled())
	      {
		this->_create_log_file(ea, _file_name);
		(*this->_log_file) 	<< ea.gen() 
					<< " Eval:" << ea.container().total_eval()
					<< " cell_fitness_Total_original_quality:" << _quality 
					<< " cell_fitness_Total_replaced_quality:" << _moved_quality 
					<<std::endl;
	      }
	  }
      }
      
      void show(std::ostream& os, size_t k) { os<<"Archive_quality:"<<_quality<<std::endl; }
      
      const double quality() const { return _quality; }
      const std::string file_name() const { return _file_name; }
      const std::string type() const { return _type; }
      
      template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
        ar & BOOST_SERIALIZATION_NVP(_quality);
      }
      
    protected:
      double _quality = 0;
      double _moved_quality = 0;
      std::string _file_name = "total_quality.dat";
      std::string _type = "graph";
    };
  } //stat
} //sferes
