#include "igclib/geometry.hpp"
#include "igclib/geopoint.hpp"
#include <string>

Cylinder::Cylinder(GeoPoint &center, double radius) {
  this->center = center;
  this->radius = radius;
}

bool Cylinder::contains(const GeoPoint &point) const {
  (void)point;
  return true;
}