#pragma once

#include "boost/geometry.hpp"
#include "igclib/geopoint.hpp"
#include "igclib/time.hpp"
#include <iterator>
#include <unordered_map>

typedef boost::geometry::model::linestring<GeoPoint> linestring_t;

class PointCollection {
public:
  PointCollection();
  void insert(const Time &t, const GeoPoint &p);
  int size() const { return this->points.size(); };
  void box() const;
  auto begin() const { return this->points.begin(); };
  auto end() const { return this->points.end(); };

private:
  std::unordered_map<Time, GeoPoint> points;
  linestring_t linestring;
};