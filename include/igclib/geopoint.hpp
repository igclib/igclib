#pragma once

#include "boost/geometry.hpp"
#include "boost/geometry/geometries/register/point.hpp"
#include <string>

class GeoPoint {
public:
  GeoPoint(const std::string &str);
  GeoPoint();
  

  double lat;
  double lon;
  double alt;
};

BOOST_GEOMETRY_REGISTER_POINT_2D(GeoPoint, double, cs::cartesian, lat, lon);
