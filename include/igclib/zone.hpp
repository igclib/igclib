#pragma once

#include "igclib/geopoint.hpp"
#include "igclib/geometry.hpp"
#include "igclib/pointcollection.hpp"
#include <ostream>
#include <string>
#include <vector>

class Zone {
  friend class Airspace;

public:
  Zone(const std::vector<std::string> &openair_record);
  std::vector<GeoPoint> contained_points(const PointCollection& points) const;
  friend std::ostream &operator<<(std::ostream &os, const Zone &z);

private:
  std::string name;
  std::string cls;

  int ceiling;
  int floor;

  bool ceiling_ground_relative;
  bool floor_ground_relative;

  std::vector<Geometry> geometries;
};