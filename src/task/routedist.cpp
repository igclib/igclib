#include <igclib/geopoint.hpp>
#include <igclib/logging.hpp>
#include <igclib/routedist.hpp>
#include <vector>

RouteDist::RouteDist(const std::vector<GeoPoint> &centers,
                     const std::vector<std::size_t> &radii)
    : m_centers(centers), m_radii(radii) {}

double RouteDist::value(const Eigen::VectorXd &theta) {
  double distance = 0;
  GeoPoint last_proj = m_centers.front().project(m_radii.front(), theta(0, 0));
  GeoPoint next_proj;
  for (std::size_t i = 1; i < m_centers.size(); i++) {
    next_proj = m_centers.at(i).project(m_radii.at(i), theta(i, 0));
    distance += last_proj.distance(next_proj);
    last_proj = next_proj;
  }
  // logging::debug({std::to_string(distance)});
  return distance;
}