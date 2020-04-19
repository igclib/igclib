#include <dlib/optimization.h>
#include <igclib/geopoint.hpp>
#include <igclib/logging.hpp>
#include <igclib/route.hpp>
#include <vector>

vec Route::gen_init(std::size_t n) {
  vec v(n);
  for (std::size_t i = 0; i < n; ++i) {
    v(i) = 0;
  }
  return v;
}

Route::Route(const GeoPoint &position, const std::deque<GeoPoint> &centers,
             const std::deque<std::size_t> &radii)
    : Route(position, centers, radii, Route::gen_init(centers.size())) {}

Route::Route(const GeoPoint &position, const std::deque<GeoPoint> &centers,
             const std::deque<std::size_t> &radii, const vec &init_vec)
    : m_position(position), m_centers(centers),
      m_radii(radii), m_opt_points{position}, m_opt_vec(init_vec),
      m_opt_distance(0), m_center_distance(0) {

  if (centers.size() != radii.size()) {
    throw std::runtime_error("size of centers and radii differ");
  }

  this->optimize();
  GeoPoint pos(position);
  GeoPoint prev_center(position);
  GeoPoint proj;
  double opt_leg;
  double center_leg;
  for (std::size_t i = 0; i < this->m_centers.size(); ++i) {
    proj = this->m_centers.at(i).project(this->m_opt_theta.at(i),
                                         this->m_radii.at(i));
    this->m_opt_points.push_back(proj);
    center_leg = prev_center.distance(this->m_centers.at(i));
    opt_leg = proj.distance(pos);

    this->m_opt_legs.push_back(opt_leg);
    this->m_center_legs.push_back(center_leg);
    this->m_center_distance += center_leg;
    this->m_opt_distance += opt_leg;

    prev_center = this->m_centers.at(i);
    pos = proj;
  }
}

double Route::operator()(const vec &theta) const {
  return this->disteval(theta);
}

void Route::optimize() {

  dlib::find_min_using_approximate_derivatives(
      dlib::bfgs_search_strategy(), dlib::objective_delta_stop_strategy(),
      *this, this->m_opt_vec, -1);

  this->m_opt_theta =
      std::vector<double>(this->m_opt_vec.begin(), this->m_opt_vec.end());
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