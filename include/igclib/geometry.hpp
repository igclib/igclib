#pragma once

#include "igclib/geopoint.hpp"

class Geometry {
public:
  virtual bool contains(const GeoPoint &point) const = 0;
};

class Cylinder : public Geometry {
  public:
  bool contains(const GeoPoint &point) const;
};

class Polygon : public Geometry {
  public:
  bool contains(const GeoPoint &point) const;
};

class Sector : public Geometry {
  public:
  bool contains(const GeoPoint &point) const;
};