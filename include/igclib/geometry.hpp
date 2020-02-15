#pragma once

#include "igclib/geopoint.hpp"
#include <string>
#include <vector>

typedef boost::geometry::model::box<GeoPoint> box_t;
typedef boost::geometry::model::polygon<GeoPoint> polygon_t;

class Geometry {
public:
  virtual bool contains(const GeoPoint &point) const = 0;
  const box_t &bounding_box() const { return this->bbox; };

protected:
  box_t bbox;
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
  polygon_t polygon;
};

class Sector : public Geometry {
public:
  Sector(const GeoPoint &center, int radius, double angle_start,
         double angle_end);
  bool contains(const GeoPoint &point) const;

private:
  GeoPoint center;
  int radius;
  double angle_start;
  double angle_end;
};