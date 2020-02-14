#include "igclib/geometry.hpp"
#include "GeographicLib/Geodesic.hpp"
#include "boost/geometry.hpp"
#include "igclib/geopoint.hpp"
#include <string>
#include <vector>

Cylinder::Cylinder(GeoPoint &center, double radius) {
  this->center = center;
  this->radius = radius;
  polygon_t edges;
  const GeographicLib::Geodesic geod = GeographicLib::Geodesic::WGS84();
  for (int i = 0; i < 4; i++) {
    double lat;
    double lon;
    geod.Direct(this->center.lat, this->center.lon, i * 90, this->radius, lat,
                lon);
    boost::geometry::append(edges, GeoPoint(lat, lon, 0, 0));
  }
  boost::geometry::envelope(edges, this->bounding_box);
}

bool Cylinder::contains(const GeoPoint &point) const {
  double distance_to_center = this->center.distance(point);
  return distance_to_center <= this->radius;
}

Polygon::Polygon(const std::vector<GeoPoint> &vertices) {
  boost::geometry::assign_points(this->polygon, vertices);
  boost::geometry::envelope(this->polygon, this->bounding_box);
}

bool Polygon::contains(const GeoPoint &point) const {
  return boost::geometry::within(point, this->polygon);
}