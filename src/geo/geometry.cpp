#include "igclib/geometry.hpp"
#include "boost/geometry.hpp"
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

Polygon::Polygon(const std::vector<GeoPoint> &vertices) {
  boost::geometry::assign_points(this->polygon, vertices);
  boost::geometry::envelope(this->polygon, this->bounding_box);
}

bool Polygon::contains(const GeoPoint &point) const {
  return boost::geometry::within(point, this->polygon);
}