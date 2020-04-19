#pragma once

#include <deque>
#include <dlib/optimization.h>
#include <igclib/geopoint.hpp>
#include <igclib/json.hpp>
#include <vector>

using vec = dlib::matrix<double, 0, 1>;

// TODO TaskRoute and FlyRoute to limit unused computations for race ?
class Route {

public:
  Route() = default;
  Route(const GeoPoint &position, const std::deque<GeoPoint> &centers,
        const std::deque<std::size_t> &radii);
  Route(const GeoPoint &position, const std::deque<GeoPoint> &centers,
        const std::deque<std::size_t> &radii, const vec &init_vec);
  double operator()(const vec &theta) const;
  double optimal_distance() const { return this->m_opt_distance; }
  const std::vector<double> &legs() const { return this->m_opt_legs; }
  const vec &opt_vec() const { return this->m_opt_vec; }
  static vec gen_init(std::size_t n);

  json to_json() const;

protected:
  double disteval(const vec &theta) const;
  void optimize();

  // current position
  GeoPoint m_position;

  // remaining turnpoints
  std::deque<GeoPoint> m_centers;
  std::deque<std::size_t> m_radii;

  // optimized points
  std::vector<GeoPoint> m_opt_points;

  // optimized angle vector
  std::vector<double> m_opt_theta;
  vec m_opt_vec;

  // leg distances
  std::vector<double> m_opt_legs;
  std::vector<double> m_center_legs;

  // total distances
  double m_opt_distance;
  double m_center_distance;
};