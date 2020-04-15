#include <igclib/geopoint.hpp>
#include <igclib/logging.hpp>
#include <igclib/routedist.hpp>
#include <vector>

RouteDist::RouteDist(const GeoPoint &position,
                     const std::vector<GeoPoint> &centers,
                     const std::vector<std::size_t> &radii)
    : m_centers(centers), m_radii(radii), m_position(position) {}

double RouteDist::operator()(const dlib::matrix<double, 0, 1> &theta) const {
  double distance = 0;
  double new_leg = 0;
  double angle;
  GeoPoint pos = this->m_position;
  GeoPoint proj;
  for (std::size_t i = 0; i < m_centers.size(); i++) {
    angle = theta(i);
    proj = m_centers.at(i).project(angle, m_radii.at(i));
    new_leg = proj.distance(pos);
    distance += new_leg;
    pos = proj;
  }
  // logging::debug({std::to_string(distance)});
  return distance;
}