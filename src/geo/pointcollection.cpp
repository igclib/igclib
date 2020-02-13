#include "igclib/pointcollection.hpp"
#include "boost/geometry.hpp"
#include <iostream>

PointCollection::PointCollection() { this->linestring = linestring_t(); }

void PointCollection::insert(const Time &t, const GeoPoint &p) {
  this->points.emplace(t, p);
  boost::geometry::append(this->linestring, p);
}

void PointCollection::box() const {
  // compute bounding box
  double min_lat = 90.0;
  double max_lat = -90.0;
  double min_lon = 180.0;
  double max_lon = -180.0;
  double min_alt = 10000; // this is arbitrary
  double max_alt = 0;
  for (auto &it : this->points) {
    if (it.second.lat < min_lat) {
      min_lat = it.second.lat;
    }
    if (it.second.lat > max_lat) {
      max_lat = it.second.lat;
    }
    if (it.second.lon < min_lon) {
      min_lon = it.second.lon;
    }
    if (it.second.lon > max_lon) {
      max_lon = it.second.lon;
    }
    if (it.second.alt < min_alt) {
      min_alt = it.second.alt;
    }
    if (it.second.alt > max_alt) {
      max_alt = it.second.alt;
    }
  }
#ifndef NDEBUG
  std::cerr << this->points.size() << " points in flight" << std::endl;
  std::cerr << min_lat << " : " << max_lat << " - " << min_lon << " : "
            << max_lon << " - " << min_alt << " : " << max_alt << std::endl;
  std::cerr << "Linestring of length "
            << boost::geometry::length(this->linestring) << std::endl;
#endif
}
