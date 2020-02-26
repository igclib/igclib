#include <igclib/geopoint.hpp>
#include <igclib/pointset.hpp>
#include <vector>

PointSet::PointSet(int idx_start, int idx_stop, std::vector<GeoPoint> bbox) {
  this->idx_start = idx_start;
  this->idx_stop = idx_stop;
  this->bbox = bbox;
}

bool PointSet::can_be_split() const {
  // TODO determine proper stopping condition
  return (this->idx_stop > this->idx_start + 2);
}

double PointSet::max_distance(const PointSet &other_set) const {
  double max_distance = 0;
  double current_distance = 0;
  for (const GeoPoint &p1 : this->bbox) {
    for (const GeoPoint &p2 : other_set.bbox) {
      current_distance = p1.distance(p2);
      if (current_distance > max_distance) {
        max_distance = current_distance;
      }
    }
  }
  return max_distance;
}
