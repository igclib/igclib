#pragma once

#include <dlib/optimization.h>
#include <igclib/geopoint.hpp>
#include <igclib/json.hpp>
#include <vector>

// TODO TaskRoute and FlyRoute to limit unused computations for race ?
class Route {
  typedef dlib::matrix<double, 0, 1> vec;

public:
  Route(){};
  Route(const GeoPoint &position, const std::vector<GeoPoint> &centers,
        const std::vector<std::size_t> &radii);
  double operator()(const vec &theta) const;

  json to_json() const;

protected:
  double disteval(const vec &theta) const;
  void optimize();

  // current position
  GeoPoint m_position;

  // remaining turnpoints
  std::vector<GeoPoint> m_centers;
  std::vector<std::size_t> m_radii;

  // optimized points
  std::vector<GeoPoint> m_opt_points;

  // optimized angle vector
  std::vector<double> m_opt_theta;

  // leg distances
  std::vector<double> m_opt_legs;
  std::vector<double> m_center_legs;

  // total distances
  double m_opt_distance;
  double m_center_distance;
};