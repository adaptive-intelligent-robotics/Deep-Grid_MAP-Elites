namespace sferes {
  namespace stat {

    // Save any fitness archive parent class
    SFERES_STAT(Archive_save, Stat)
    {
    public:

      template <typename E> void refresh(const E& ea)
      {
	if (ea.gen() % Params::pop::dump_period == 0)
	  {
	    _file_name = _name_prefixe + std::to_string(ea.gen()) + ".dat";
	    _write_container(ea, _file_name);
	  }
      }
      
      const std::string file_name() const { return _file_name; }
      const std::string type() const { return _type; }
      
    protected:

      std::string _file_name;
      std::string _type = "archive";
      std::string _name_prefixe;

      virtual bool _write_moved_cell() = 0;
      virtual bool _use_moved_fitness() = 0;
      
      template <typename EA>
	void _write_container(const EA& ea, std::string file_name)
      {
	// Opening file
	std::cout<<"Writting..."<<file_name<<std::endl;
	std::ofstream open_file;
	open_file.open(ea.res_dir() + "/" + file_name);
	
	// Writting each filled cell of the archive
	auto filled_content = ea.container().get_filled_content();
	int offset = 0;
	for(auto cell : filled_content)
	  {
	    // Read corresponding descriptor
	    std::vector<double> desc_center = ea.container().get_center(cell);
	    
	    if (_write_moved_cell() 
		|| !boost::fusion::at_c<PHANTOM_MODIFIER>(ea.fit_modifier()).moved(ea, desc_center))
	      {
	       if ((!_use_moved_fitness()
		   && !boost::fusion::at_c<PHANTOM_MODIFIER>(ea.fit_modifier()).is_empty(ea, desc_center))
		   || (_use_moved_fitness()
		   && !boost::fusion::at_c<PHANTOM_MODIFIER>(ea.fit_modifier()).is_moved_empty(ea, desc_center)) )
		 {
		   
		   // Add descriptor to line
		   std::string write = std::to_string(offset) + " ";
		   for (size_t dim = 0; dim < desc_center.size(); ++dim)
		     write += std::to_string(desc_center[dim])+ " ";
		   
		   // Add fitness to line
		   if (_use_moved_fitness())
		     write += std::to_string(boost::fusion::at_c<PHANTOM_MODIFIER>(ea.fit_modifier())
					     .get_moved_fitness(ea, desc_center))+ " ";
		   else
		     write += std::to_string(boost::fusion::at_c<PHANTOM_MODIFIER>(ea.fit_modifier())
					     .get_fitness(ea, desc_center)) + " ";
		   
		   // Write it in the file 
		   open_file<<write<<std::endl;
		   offset++;
		 }
	      }
	  }
      }
    };
    
    // Save archive original
    SFERES_STAT(Archive_original_save, Archive_save)
    {
    public:
      Archive_original_save() { this->_name_prefixe = "Archive_Fitness-"; }
    protected:
      bool _write_moved_cell() override { return true; };
      bool _use_moved_fitness() override { return false; };
    };
    
    // Save archive moved
    SFERES_STAT(Archive_moved_save, Archive_save)
    {
    public:
      Archive_moved_save() { this->_name_prefixe = "Archive_Fitness_stable-"; }
    protected:
      bool _write_moved_cell() override { return true; };
      bool _use_moved_fitness() override { return true; };
    };
    
    // Save archive empty
    SFERES_STAT(Archive_empty_save, Archive_save)
    {
    public:
      Archive_empty_save() { this->_name_prefixe = "Archive_Fitness_empty-"; }
    protected:
      bool _write_moved_cell() override { return false; };
      bool _use_moved_fitness() override { return false; };
    };
    
  } //stat
} //sferes
