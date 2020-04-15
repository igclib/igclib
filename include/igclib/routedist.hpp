#include <cppoptlib/meta.h>
#include <cppoptlib/problem.h>
#include <cppoptlib/solver/bfgssolver.h>
#include <igclib/geopoint.hpp>
#include <vector>

class RouteDist : public cppoptlib::Problem<double> {
public:
  RouteDist(const std::vector<GeoPoint> &centers,
            const std::vector<std::size_t> &radii);
  double value(const Eigen::VectorXd &theta);

protected:
  std::vector<GeoPoint> m_centers;
  std::vector<std::size_t> m_radii;
};