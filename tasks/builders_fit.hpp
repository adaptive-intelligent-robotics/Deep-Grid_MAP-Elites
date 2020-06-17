// File used to define all functions common to all stochastic fitness
template <typename Params> class Fit_stochastic: 
  public Fit_stochastic_base<Params>
{

public:

  typedef std::vector<double> desc_t;
 
  // Rewrite eval function
  template <typename Indiv> void eval(Indiv & ind)
  {
    // Preprocess and set all needed values
    this->_preprocess(ind.data());

    // Simulate
    float value = 0;
    desc_t desc;
    std::tie(value, desc, this->_dead) = simulate(ind);
    
    // Set value, desc and dead
    _set_value(value);
    _set_desc(desc);
  };
 

  // Rewrite eval function for n parallel evaluations
  template<typename Indiv> void eval(Indiv & ind, size_t n)
  {
    // Preprocess and set all needed values
    this->_preprocess(ind.data());

    // Simulate
    auto values = simulate(ind, n);

    // Set value, desc and dead
    for (size_t i=0; !this->_dead && i<n; i++)
      {
	if (std::get<2>(values)[i])
	  this->_dead = true;
	this->_set_value(std::get<0>(values)[i]);
	this->_set_desc(std::get<1>(values)[i]);
      }
  };

  // Wrap simulate to add noise and call it with the control values
  template <typename Indiv>
   std::tuple<float, std::vector<double>, bool > simulate(const Indiv &ind) const
  {
    float value;
    std::vector<double> desc;
    bool dead;
    std::tie(value, desc, dead) = Fit_stochastic_base<Params>::simulate(ind.data());
    
    // Normalise
    _normalise(value);
    
    // Apply noise
    this->_apply_noise_desc(desc, value);
    this->_apply_noise_value(value);
    
    // Test if dead
    if(dead || this->_is_dead(value, desc))
      dead = true;
    
    // Return values
    return {value, desc, dead};
  };

  // Perform parallel eval of simulate
  template<typename Indiv> 
    std::tuple<std::vector<float>, std::vector<std::vector<double>>, std::vector<bool>> 
      simulate(const Indiv &ind, size_t n) const
  {
    std::vector<std::vector<double>> descs(n);
    std::vector<float> values(n);
    std::vector<bool> deads(n);
    tbb::parallel_for(size_t(0), n, size_t(1), [&](size_t i)
		      {
		        std::tuple<float, std::vector<double>, bool > results = this->simulate(ind);
			values[i] = std::get<0>(results);
			descs[i] = std::get<1>(results);
			deads[i] = std::get<2>(results);
		      });
    return {values, descs, deads};
  };

  // Set an indiv as dead for being out of the container
  void out_of_container() { this->_dead = true; };
 

  // Descriptor
  const std::vector<desc_t>& desc_values() const { return _desc_values; };
  const size_t desc_number() const { return _desc_number; };
  const std::vector<double> desc_variance() const { return _desc_variance; };
  const desc_t first_desc() const {  return _first_desc; };

  // Fitness
  const std::vector<float>& value_values() const { return _value_values; };
  const size_t value_number() const { return _value_number; };
  const float value_variance() const { return _value_variance; };

 

protected:

  // Descriptor
  size_t _desc_number = 0;
  desc_t _first_desc;
  std::vector<desc_t> _desc_values;
  desc_t _desc_variance;
  
  void _set_desc(desc_t & x)
  {
    assert(x.size() == Params::qd::behav_dim);
    _desc_number++;
    _desc_values.push_back(x);
    if (_desc_number==1)
      {
	this->_desc = x;
	_first_desc = x;
	_desc_variance.resize(Params::qd::behav_dim);
	_desc_variance.assign(Params::qd::behav_dim, 0);
      }
    else
      {	
	for (size_t i=0; i<Params::qd::behav_dim; i++)
	  {
	    float mean = 0;
	    float var = 0;
	    for (size_t j=0; j<_desc_number; j++)
	      {
		mean += _desc_values[j][i];
		var += _desc_values[j][i]*_desc_values[j][i];
	      }
	    mean /= _desc_number;
	    var /=_desc_number;
	    var -= mean*mean;
	    this->_desc[i] = mean;
	    _desc_variance[i] = var;
	  }
      }
  };
  
  
  // Fitness
  size_t _value_number = 0;
  std::vector<float> _value_values;
  float _value_variance = 0;

  void _set_value(float val) 
  { 
    _value_values.push_back(val);
    _value_number++;
    
    double mean = 0;
    double var = 0;
    for (double val : _value_values)
      {
	mean += val;
	var += val*val;
      }
    mean /= _value_number;
    var /=_value_number;
    var -= mean*mean;
    this->_value = mean;
    _value_variance = var;
  };
  
  // Normalise fitness
  void _normalise(float& value) const 
  { 
    if (Params::stats::min != Params::stats::max)
      value = (value - Params::stats::min) / (Params::stats::max - Params::stats::min); 
  };

}; //Fit_stochastic

