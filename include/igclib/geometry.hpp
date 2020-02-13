#pragma once

#include "igclib/geopoint.hpp"
#include <string>

class Geometry {
public:
  virtual bool contains(const GeoPoint &point) const = 0;
};

class Cylinder : public Geometry {
public:
  Cylinder(GeoPoint &center, double radius);
  bool contains(const GeoPoint &point) const;
private:
  GeoPoint center;
  double radius;
};

private:
  GeoPoint center;
  double radius;
};

class Polygon : public Geometry {};

class Sector : public Geometry {};