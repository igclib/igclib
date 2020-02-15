#pragma once

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

class GeoPoint {
public:
  GeoPoint() : lat(0), lon(0), alt(0), agl(0){};
  GeoPoint(double lat, double lon, int alt, int agl);
  double distance(const GeoPoint &p) const;
  json serialize() const;

  double lat;
  double lon;
  int alt;
  int agl;
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