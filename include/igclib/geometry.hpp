#pragma once

#include <igclib/geopoint.hpp>
#include <string>
#include <vector>

class Geometry {
public:
  virtual bool contains(const GeoPoint &point) const = 0;
  const boost::geometry::model::box<GeoPoint> &bounding_box() const {
    return this->bbox;
  };

protected:
  boost::geometry::model::box<GeoPoint> bbox;
};

class Cylinder : public Geometry {
public:
  Cylinder(const GeoPoint &center, double radius);
  bool contains(const GeoPoint &point) const;

private:
  GeoPoint center;
  double radius;
};

class Polygon : public Geometry {
public:
  Polygon(const std::vector<GeoPoint> &vertices);
  bool contains(const GeoPoint &point) const;

private:
  boost::geometry::model::polygon<GeoPoint> polygon;
};

class Sector : public Geometry {
public:
  Sector(const GeoPoint &center, int radius, double angle_start,
         double angle_end);
  Sector(const GeoPoint &center, const GeoPoint &p1, const GeoPoint &p2);
  bool contains(const GeoPoint &point) const;

private:
  GeoPoint center;
  int radius;
  double angle_start;
  double angle_end;
};