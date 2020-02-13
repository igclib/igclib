#include "igclib/airspace.hpp"
#include "igclib/geopoint.hpp"
#include "igclib/time.hpp"
#include "igclib/util.hpp"
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

Airspace::Airspace(const std::string &airspace_file) {
  std::ifstream f;
  f.open(airspace_file);
  if (!f.is_open()) {
    const std::string error = "Could not open file '" + airspace_file + "'";
    throw std::runtime_error(error);
  }

  std::string line;
  std::vector<std::string> record;
  while (std::getline(f, line)) {
    util::trim(line);
    if (line[0] != '*' && !line.empty()) {
      boost::to_upper(line);
      if (line.substr(0, 2) == "AC") {
        if (!record.empty()) {
          this->zones.push_back(record);
          record.clear();
        }
      }
      record.push_back(line);
    }
  }

#ifndef NDEBUG
  std::cerr << this->zones.size() << " zones in airspace" << std::endl;
#endif

  f.close();
}

const std::map<std::string, std::vector<GeoPoint>>
Airspace::infractions(const PointCollection &points) const {
  std::vector<GeoPoint> zone_infractions;
  std::map<std::string, std::vector<GeoPoint>> infractions;
  for (const Zone &zone : this->zones) {
    zone_infractions = zone.contained_points(points);
    if (!zone_infractions.empty()) {
      std::cerr << zone << std::endl;
      infractions[zone.name] = zone_infractions;
    }
  }
  return infractions;
}