#include "igclib/geometry.hpp"
#include "igclib/geopoint.hpp"
#include <string>
#include <vector>

// TODO : initialize bounding_box attribute in each derived class ctor
// TODO : implement contains for each derived class
Cylinder::Cylinder(GeoPoint &center, double radius) {
  this->center = center;
  this->radius = radius;
}

bool Cylinder::contains(const GeoPoint &point) const {
  (void)point;
  return true;
}

Polygon::Polygon(std::vector<GeoPoint> &vertices) { (void)vertices; }

bool Polygon::contains(const GeoPoint &point) const {
  (void)point;
  return true;
}