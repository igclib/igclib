#include <dlib/optimization.h>
#include <igclib/geopoint.hpp>
#include <igclib/logging.hpp>
#include <igclib/route.hpp>
#include <vector>

Route::Route(const GeoPoint &position, const std::vector<GeoPoint> &centers,
             const std::vector<std::size_t> &radii)
    : m_centers(centers), m_radii(radii),
      m_position(position), m_opt_points{position} {
  this->optimize();
  GeoPoint pos = position;
  GeoPoint prev_center = position;
  GeoPoint proj;
  for (std::size_t i = 0; i < this->m_centers.size(); ++i) {
    proj = this->m_centers.at(i).project(this->m_opt_theta.at(i),
                                         this->m_radii.at(i));
    this->m_opt_points.push_back(proj);
    this->m_opt_legs.push_back(proj.distance(pos));
    this->m_center_legs.push_back(prev_center.distance(this->m_centers.at(i)));
    prev_center = this->m_centers.at(i);
    pos = proj;
  }
  this->m_opt_distance =
      std::accumulate(m_opt_legs.begin(), m_opt_legs.end(), 0);
  this->m_center_distance =
      std::accumulate(m_center_legs.begin(), m_center_legs.end(), 0);
}

double Route::operator()(const vec &theta) const {
  return this->disteval(theta);
}

void Route::optimize() {
  vec theta(this->m_centers.size());
  for (std::size_t i = 0; i < this->m_centers.size(); i++) {
    theta(i) = 0; // TODO better first guess ?
  }
  dlib::find_min_using_approximate_derivatives(
      dlib::bfgs_search_strategy(), dlib::objective_delta_stop_strategy(1e-7),
      *this, theta, -1);
  this->m_opt_theta = std::vector<double>(theta.begin(), theta.end());
}

double Route::disteval(const vec &theta) const {
  double distance = 0;
  GeoPoint pos = this->m_position;
  GeoPoint proj;
  for (std::size_t i = 0; i < this->m_centers.size(); i++) {
    proj = this->m_centers.at(i).project(theta(i), this->m_radii.at(i));
    distance += proj.distance(pos);
    pos = proj;
  }
  return distance;
}

json Route::to_json() const {
  json j;
  j["opt_dist"] = this->m_opt_distance;
  j["center_dist"] = this->m_center_distance;
  j["center_legs"] = this->m_center_legs;
  j["opt_legs"] = this->m_opt_legs;
  for (const GeoPoint &p : this->m_opt_points) {
    j["opt_points"].push_back(p.to_json());
  }
  return j;
}