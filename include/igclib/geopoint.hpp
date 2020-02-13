#pragma once

#include "boost/geometry.hpp"
#include "boost/geometry/geometries/register/point.hpp"
#include <string>

class GeoPoint {
public:
  GeoPoint() : lat(-1), lon(-1), alt(-1), ground_alt(-1){};

  double lat;
  double lon;
  double alt;
  double ground_alt;
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