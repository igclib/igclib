#pragma once

#include "igclib/geopoint.hpp"

class Geometry {
public:
  virtual bool contains(const GeoPoint &point) const;
};
