#ifndef QD_CONTAINER_CELLS_BASE
#define QD_CONTAINER_CELLS_BASE

namespace sferes {
  namespace qd {
    namespace container {
      template <typename Phen, typename Params> class Cell_base {
	
       public:

	typedef boost::shared_ptr<Phen> indiv_t;
	typedef typename std::vector<indiv_t> pop_t;

        Cell_base() 
	{ 
	  _cell.reserve(_deep_max); 
	  _indexs.reserve(_deep_max); 
	  std::iota(_indexs.begin(), _indexs.end(), 0);
	};

	// Traditional operators
	template <typename Index> const indiv_t operator[](Index index) const { return _cell[index]; };
	size_t size() { return _cell.size(); };

	void replace(size_t index, indiv_t new_indiv) { _cell[index] = new_indiv; };
	void push_back(indiv_t indiv) { _cell.push_back(indiv); };
	void pop_back() { _cell.pop_back(); };
	
	void insert(size_t index, indiv_t indiv) { _cell.insert(_cell.begin() + index, indiv); };
	void erase(size_t index) { _cell.erase(_cell.begin() + index); };

	void swap(size_t index1, size_t index2) 
	{ std::iter_swap(_cell.begin() + index1, _cell.begin() + index2); };

	// Update cells
	void update_cell() {};

      protected:

	pop_t _cell;
	std::vector<size_t> _indexs;
	const size_t _deep_max = Params::stochastic::deep_size;

      };
    } //container
  } //qd
} //sferes
#endif
