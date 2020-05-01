#pragma once

#include <deque>
#include <igclib/geopoint.hpp>
#include <igclib/json.hpp>
#include <optimization/bfgs.hpp>
#include <vector>

// TODO TaskRoute and FlyRoute to limit unused computations for race ?
class Route {

public:
  Route() = default;
  Route(const GeoPoint &position, const std::deque<GeoPoint> &centers,
        const std::deque<std::size_t> &radii);

  Route(const GeoPoint &position, const std::deque<GeoPoint> &centers,
        const std::deque<std::size_t> &radii, const Eigen::VectorXd &init_vec);

  double optimal_distance() const { return this->m_opt_distance; }
  const std::vector<double> &legs() const { return this->m_opt_legs; }
  const Eigen::VectorXd &opt_theta() const { return this->m_opt_theta; }

  json to_json() const;

protected:
  void optimize();
  double disteval(const Eigen::VectorXd &theta) const;

  // current position
  GeoPoint m_position;

  // remaining turnpoints
  std::deque<GeoPoint> m_centers;
  std::deque<std::size_t> m_radii;

  // optimized points
  std::vector<GeoPoint> m_opt_points;

  // optimized angle vector
  Eigen::VectorXd m_opt_theta;

  // leg distances
  std::vector<double> m_opt_legs;
  std::vector<double> m_center_legs;

  // total distances
  double m_opt_distance;
  double m_center_distance;
};