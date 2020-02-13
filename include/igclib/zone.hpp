#pragma once

#include "igclib/geopoint.hpp"
#include <vector>
#include <ostream>
#include <string>

class Zone {
public:
  Zone(const std::vector<std::string> &openair_record);
  bool contains(const GeoPoint &point) const;
  friend std::ostream &operator<<(std::ostream &os, const Zone &z);

private:
  std::string name;
  std::string cls;
  std::string comment;

  int ceiling;
  int floor;

  bool ceiling_ground_relative;
  bool floor_ground_relative;

  std::vector<GeoPoint> polygon;
};