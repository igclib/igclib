#pragma once

#include "igclib/geopoint.hpp"
#include <string>
#include <vector>

typedef boost::geometry::model::box<GeoPoint> box_t;
typedef boost::geometry::model::polygon<GeoPoint> polygon_t;

class Geometry {
public:
  virtual bool contains(const GeoPoint &point) const = 0;

  polygon_t bounding_box;
};

class Cylinder : public Geometry {
public:
  Cylinder(GeoPoint &center, double radius);
  bool contains(const GeoPoint &point) const;

private:
  GeoPoint center;
  double radius;
};

class Polygon : public Geometry {
public:
  Polygon(std::vector<GeoPoint> &vertices);
  bool contains(const GeoPoint &point) const;
};

class Sector : public Geometry {};