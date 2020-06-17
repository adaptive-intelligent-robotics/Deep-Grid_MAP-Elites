#ifndef QD_CONTAINER_CELLS_DEFAULT
#define QD_CONTAINER_CELLS_DEFAULT

#include "default.hpp"

namespace sferes {
  namespace qd {
    namespace container {
      template <typename Phen, typename Params> class Cell_default : public Cell_base<Phen, Params> {
	
       public:

	typedef boost::shared_ptr<Phen> indiv_t;
	typedef typename std::vector<indiv_t> pop_t;
	typedef std::vector<double> desc_t;
	typedef std::vector<double> coord_t;

	// Get descriptor of a given index in cell
	desc_t get_desc(const indiv_t& indiv) const {return _desc(indiv); };
	desc_t get_desc(const size_t index) const { return get_desc(this->_cell[this->_indexs[index]]); };
	desc_t get_desc() const { return get_center(); };

	// Return the correct coordinates of the cell or of an indiv
	coord_t get_coord() const { return _coord(get_desc(this->_cell[0])); };
	coord_t get_coord(const indiv_t indiv) const { return _coord(get_desc(indiv)); };
	coord_t get_coord(const desc_t desc) const { return _coord(desc); };

	// Return the coordinate of the center of the cell or of an indiv
	desc_t get_center() const { return _center(get_desc(this->_cell[0])); };
	desc_t get_center(const indiv_t indiv) const { return _center(get_desc(indiv)); };
	desc_t get_center(const desc_t desc) const { return _center(desc); };

	// Return if a cell is dead from being out of limit
	bool is_out(const desc_t desc) const { return _is_out(desc); };
	bool is_out(const indiv_t indiv) const { return _is_out(get_desc(indiv)); };

	// Return the difference in number of cells
	size_t get_cell_difference(const desc_t desc) const 
	{ return _cell_diff(get_desc(this->_cell[0]), desc); };
	size_t get_cell_difference(const desc_t desc1, const desc_t desc2) const
	{ return _cell_diff(desc1, desc2); };
	size_t get_cell_difference(const indiv_t indiv) const 
	{ return _cell_diff(get_desc(this->_cell[0]), get_desc(indiv)); };
	size_t get_cell_difference(const indiv_t indiv1, const indiv_t indiv2) const
	{ return _cell_diff(get_desc(indiv1), get_desc(indiv2)); };

       protected:
  	size_t _n_cells = Params::qd::grid_shape(0); //Resolution of the grid
  	double _radius = Params::archive::radius; //size of the outer circle
  	size_t _n_circles = std::round(_radius / (2 * M_PI * _radius / (double) _n_cells)); //Total num inner circles
  	double _seg_length =  _radius/(_n_circles+0.5); //length of cells between the cells of the outer ring

	// Method to overwrite depending on the algo
	virtual desc_t _desc(const indiv_t& indiv) const = 0;

#ifndef POLAR_COORD //non polar archive

	// Is the cell out of the grid
	bool _is_out (const desc_t desc) const {return false; };

	// Give coordinates of a cell
  	coord_t _coord(const desc_t desc) const { return desc; };

	// Give center of a cell
  	desc_t _center(const desc_t desc) const
	{
          desc_t center;
     	  for (size_t i = 0; i < desc.size(); ++i)
	    center.push_back( (float)round(std::min(1.0, desc[i]) * (float)(Params::qd::grid_shape(i) - 1))
		/ (float)(Params::qd::grid_shape(i) - 1));
           return center; 
	};

	// Give distance in cell
	size_t _cell_diff(const desc_t desc1, const desc_t desc2) const
	{
	  size_t diff = 0;
	  for (size_t i = 0; i < desc1.size(); i++)
	    diff = std::max(diff,
		(size_t) std::abs( round( std::min(1.0, desc1[i]) * (Params::qd::grid_shape(i) - 1) )
				 - round( std::min(1.0, desc2[i]) * (Params::qd::grid_shape(i) - 1) ) ) );
	  return diff;
	};


#else //polar archive

	// _polar_coord and _cartes_coord allow to switch from one coordinate to the other
  	desc_t _polar_coord(const desc_t desc) const
  	 {
    	   double r = sqrt(pow(desc[0] - 0.5, 2) + pow(desc[1] - 0.5, 2));
	   double t = atan2(desc[1] - 0.5, desc[0] - 0.5);
	   if(t<0)
      	     t+=2*M_PI;
	   return { r, t };
	 };
	desc_t _cartes_coord(const desc_t desc) const
	{ return { desc[0] * cos(desc[1]) + 0.5, desc[0] * sin(desc[1]) + 0.5 }; };

	// Is the cell out of the grid
	bool _is_out (const desc_t desc) const {return _polar_coord(desc)[0] > _radius; };

	// Give coordinates of a cell
  	coord_t _coord(const desc_t desc) const
  	 {
    	   desc_t desc_polar = _polar_coord(desc);
	   size_t d1 = std::min(_n_circles, (size_t) std::round(desc_polar[0] / _seg_length));
	   size_t nb_points = _get_nb_points(d1);
	   size_t d2 = std::round(desc_polar[1] * (double) nb_points / (2 * M_PI));
	   if (d2 == nb_points)
		   d2 = 0;
	   return {(double) d1 / (double) (_n_cells - 1), (double) d2 / (double) (_n_cells - 1)};
	 };
  	size_t _get_nb_points(const size_t d1) const
	{ return std::round((double) _n_cells * ((double) d1+0.5) / ((double) _n_circles + 0.5)); };

	// Give center of a cell
  	desc_t _center(const desc_t desc) const 
	{
	  coord_t coord = _coord(desc);
	  desc_t desc_polar = { std::round(coord[0] * (double) (_n_cells - 1)) * _seg_length,
				std::round(coord[1] * (double) (_n_cells - 1)) * 2 * M_PI 
				  / (double) _get_nb_points(std::round(coord[0] * (double) (_n_cells - 1))) };
	  return _cartes_coord(desc_polar);
	};

	// Return the distance in cell
	size_t _distance_border(const coord_t coord) const
	{ return std::abs( _n_circles - round(coord[0] * (double)(_n_cells-1)) ); };
	size_t _distance(const coord_t coord1, coord_t coord2) const
	{ return std::max( std::abs( round(coord1[0]*(double)(_n_cells-1)) - round(coord2[0]*(double)(_n_cells-1))),
			   std::abs( round(coord1[1]*(double)(_n_cells-1)) - round(coord2[1]*(double)(_n_cells-1))));};
	size_t _cell_diff(const desc_t desc1, const desc_t desc2) const
	{
	  bool dead1 = is_out(desc1);
	  bool dead2 = is_out(desc2);
	  if (dead1 && dead2)
	    return 0;
	  if (dead1)
	    return _distance_border(_coord(desc2));
	  if (dead2)
	    return _distance_border(_coord(desc1));
	  return  _distance(_coord(desc1), _coord(desc2));
	};


#endif

      };
    } //container
  } //qd
} //sferes

#endif
