#include "igclib/pointcollection.hpp"
#include "boost/geometry.hpp"
#include <iostream>

PointCollection::PointCollection() {
  // why is  this necessary ?
  this->linestring = linestring_t();
}

void PointCollection::insert(const Time &t, const GeoPoint &p) {
  this->points.emplace(t, p);
  boost::geometry::append(this->linestring, p);
}

void PointCollection::close() {
  boost::geometry::envelope(this->linestring, this->bounding_box);
}

void PointCollection::box() const {
#ifndef NDEBUG
  std::cerr << this->points.size() << " points in flight" << std::endl;
  std::cerr << "Bbox " << boost::geometry::dsv(bounding_box) << std::endl;
#endif
  // return this->bounding_box;
}