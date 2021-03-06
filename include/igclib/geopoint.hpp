#pragma once

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <igclib/json.hpp>
#include <string>

class GeoPoint {
public:
  GeoPoint() : lat(0), lon(0), alt(0), agl(0){};
  GeoPoint(double lat, double lon, int alt);
  GeoPoint(double lat, double lon, int alt, int agl);
  bool operator==(const GeoPoint &other) const;
  std::string id() const; // used for caching distances
  double distance(const GeoPoint &p) const;
  double heading(const GeoPoint &p) const;
  GeoPoint project(double distance, double heading) const;
  json to_json() const;

  double lat;
  double lon;
  int alt;
  int agl;

protected:
  std::string m_id;
};

BOOST_GEOMETRY_REGISTER_POINT_2D(GeoPoint, double, cs::cartesian, lat, lon)

class IGCPoint : public GeoPoint {
public:
  IGCPoint(const std::string &str);
};

class OpenAirPoint : public GeoPoint {
public:
  OpenAirPoint(const std::string &str);
};