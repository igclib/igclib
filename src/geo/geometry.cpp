#include <GeographicLib/Geodesic.hpp>
#include <boost/geometry.hpp>
#include <igclib/geometry.hpp>
#include <igclib/geopoint.hpp>
#include <string>
#include <vector>

Cylinder::Cylinder(const GeoPoint &center, double radius) {
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
  boost::geometry::envelope(edges, this->bbox);
}

bool Cylinder::contains(const GeoPoint &point) const {
  double distance_to_center = this->center.distance(point);
  return distance_to_center <= this->radius;
}

Polygon::Polygon(const std::vector<GeoPoint> &vertices) {
  boost::geometry::assign_points(this->polygon, vertices);
  boost::geometry::envelope(this->polygon, this->bbox);
}

bool Polygon::contains(const GeoPoint &point) const {
  return boost::geometry::within(point, this->polygon);
}

Sector::Sector(const GeoPoint &center, int radius, double angle_start,
               double angle_end) {
  // TODO optimize this bounding box estimation
  // is bissectrix projection always on bbox ?
  this->center = center;
  this->radius = radius;
  this->angle_start = angle_start;
  this->angle_end = angle_end;
  this->bbox = Cylinder(center, radius).bounding_box();
}

Sector::Sector(const GeoPoint &center, const GeoPoint &p1, const GeoPoint &p2) {
  const GeographicLib::Geodesic geod = GeographicLib::Geodesic::WGS84();
  double angle_at_point; // not used but necessary for required constructor
  double distance_p1;
  double distance_p2;
  geod.Inverse(center.lat, center.lon, p1.lat, p1.lon, distance_p1,
               this->angle_start, angle_at_point);
  geod.Inverse(center.lat, center.lon, p2.lat, p2.lon, distance_p2,
               this->angle_end, angle_at_point);
  // radius is set to furthest point
  this->radius = distance_p1 > distance_p2 ? distance_p1 : distance_p2;
  this->bbox = Cylinder(this->center, this->radius).bounding_box();
}

bool Sector::contains(const GeoPoint &point) const {
  const GeographicLib::Geodesic geod = GeographicLib::Geodesic::WGS84();
  double angle_from_center;
  double angle_at_point;
  geod.Inverse(this->center.lat, this->center.lon, point.lat, point.lon,
               angle_from_center, angle_at_point);
  if (angle_from_center >= this->angle_start &&
      angle_from_center <= this->angle_end) {
    double distance_to_center = this->center.distance(point);
    return distance_to_center <= this->radius;
  }
  return false;
}