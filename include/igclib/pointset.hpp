#pragma once

#include <igclib/geopoint.hpp>
#include <vector>

class PointSet {
public:
  PointSet(int idx_start, int idx_stop, std::vector<GeoPoint> bbox);
  PointSet(int idx_start, int idx_stop);
  PointSet(){};

  bool can_be_split() const;
  double max_distance(const PointSet &other_set) const;

  std::vector<GeoPoint> bbox;

  int idx_start;
  int idx_stop;
};