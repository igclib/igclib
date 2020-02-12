#pragma once

#include <string>
#include "boost/geometry.hpp"
#include <boost/geometry/geometries/register/point.hpp>

class GeoPoint {
public:
  GeoPoint(const std::string &str);
  GeoPoint(const GeoPoint& g);
  GeoPoint();

  double lat;
  double lon;
  double alt;
};

BOOST_GEOMETRY_REGISTER_POINT_3D(GeoPoint, double, cs::cartesian, lat, lon, alt);