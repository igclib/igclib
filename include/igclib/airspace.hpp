#pragma once

#include "igclib/geopoint.hpp"
#include "igclib/pointcollection.hpp"
#include "igclib/zone.hpp"
#include <string>
#include <vector>

class Airspace {
public:
  Airspace(const std::string &airspace_file);
  size_t size() { return this->zones.size(); };

  const std::map<std::string, std::vector<GeoPoint>>
  infractions(const PointCollection &points) const;

private:
  std::vector<Zone> zones;
};