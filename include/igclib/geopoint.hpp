#pragma once

#include "boost/geometry.hpp"
#include "boost/geometry/geometries/register/point.hpp"
#include <string>

class GeoPoint {
public:
  GeoPoint() : lat(0), lon(0), alt(0) {};

  double lat;
  double lon;
  double alt;
};

BOOST_GEOMETRY_REGISTER_POINT_2D(GeoPoint, double, cs::cartesian, lat, lon);

class IGCPoint : public GeoPoint {
public:
  IGCPoint(const std::string &str);
};