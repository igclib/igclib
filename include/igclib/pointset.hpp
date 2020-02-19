#pragma once

#include <igclib/geopoint.hpp>
#include <vector>

class PointSet {
public:
  PointSet(int idx_start, int idx_stop, int points_contained,
           std::vector<GeoPoint> bbox);

  bool can_be_split() const;

private:
  int idx_start;
  int idx_stop;
  int points_contained;
  std::vector<GeoPoint> bbox;
};