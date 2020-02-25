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