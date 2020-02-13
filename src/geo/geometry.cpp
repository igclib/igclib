#include "igclib/geometry.hpp"
#include "igclib/geopoint.hpp"
#include <string>

<<<<<<< HEAD
Cylinder::Cylinder(GeoPoint &center, double radius) {
=======
Cylinder::Cylinder(GeoPoint center, double radius) {
>>>>>>> a949842... TO SQUASH
  this->center = center;
  this->radius = radius;
}

bool Cylinder::contains(const GeoPoint &point) const {
  (void)point;
  return true;
}