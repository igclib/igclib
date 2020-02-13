#pragma once

#include "igclib/geopoint.hpp"
#include "igclib/pointcollection.hpp"
#include "igclib/zone.hpp"
#include <string>
#include <vector>

class Airspace {
public:
  Airspace(const std::string &airspace_file);
  const std::vector<GeoPoint> infractions(const PointCollection &points) const;

private:
  std::vector<Zone> zones;
};