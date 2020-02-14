#pragma once

#include "boost/geometry.hpp"
#include "igclib/geopoint.hpp"
#include "igclib/time.hpp"
#include <iterator>
#include <unordered_map>

typedef boost::geometry::model::linestring<GeoPoint> linestring_t;
typedef boost::geometry::model::box<GeoPoint> box_t;
typedef std::unordered_map<Time, GeoPoint> timepoints_t;

class PointCollection {
public:
  PointCollection();
  void insert(const Time &t, const GeoPoint &p);
  int size() const { return this->points.size(); };
  void close();
  void box() const;
  timepoints_t::const_iterator begin() const { return this->points.begin(); };
  timepoints_t::const_iterator end() const { return this->points.end(); };

private:
  timepoints_t points;
  linestring_t linestring;
  box_t bounding_box;
};