#pragma once

#include "boost/geometry.hpp"
#include "boost/geometry/geometries/register/point.hpp"
#include <string>

class GeoPoint {
public:
  GeoPoint(){};
  GeoPoint(double lat, double lon, int alt, int ground_alt);

  double lat;
  double lon;
  int alt;
  int ground_alt;
};

BOOST_GEOMETRY_REGISTER_POINT_2D(GeoPoint, double, cs::cartesian, lat, lon);

class IGCPoint : public GeoPoint {
public:
  IGCPoint(const std::string &str);
};

class OpenAirPoint : public GeoPoint {
public:
  OpenAirPoint(const std::string &str);
};