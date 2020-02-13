#include "igclib/geometry.hpp"
#include "igclib/geopoint.hpp"

bool Polygon::contains(const GeoPoint &point) const {
  (void)point;
  return true;
}