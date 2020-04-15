#include <dlib/optimization.h>
#include <igclib/geopoint.hpp>
#include <vector>

class RouteDist {
public:
  RouteDist(const GeoPoint &position, const std::vector<GeoPoint> &centers,
            const std::vector<std::size_t> &radii);
  double operator()(const dlib::matrix<double, 0, 1> &theta) const;

protected:
  std::vector<GeoPoint> m_centers;
  std::vector<std::size_t> m_radii;
  GeoPoint m_position;
};