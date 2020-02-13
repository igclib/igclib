#include "igclib/geometry.hpp"
#include "igclib/geopoint.hpp"
#include <string>
#include <vector>

Cylinder::Cylinder(GeoPoint &center, double radius) {
  this->center = center;
  this->radius = radius;
}

bool Cylinder::contains(const GeoPoint &point) const {
  (void)point;
  return true;
}

Polygon::Polygon(std::vector<GeoPoint>& vertices) {
  (void)vertices;
}

bool Polygon::contains(const GeoPoint &point) const {
  (void)point;
  return true;
}