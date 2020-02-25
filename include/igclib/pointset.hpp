#pragma once

#include <igclib/geopoint.hpp>
#include <vector>

class PointSet {
public:
  PointSet(int idx_start, int idx_stop, std::vector<GeoPoint> bbox);

  bool can_be_split() const;
  std::vector<GeoPoint> bbox;

private:
  int idx_start;
  int idx_stop;
};