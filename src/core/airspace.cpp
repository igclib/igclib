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
    // discard comments and empty lines
    if (line[0] != '*' && !line.empty()) {
      boost::to_upper(line);
      if (line.substr(0, 2) == "AC") {
        if (!record.empty()) {
          Zone zone(record);
          // zone might be empty if no geometry could be parsed from the record
          // in this case, do not add it to the index, but clear the record
          // anyway
          if (!zone.empty()) {
            box_mapping_t value =
                std::make_pair(zone.bounding_box, zones.size());
            this->index.insert(value);
            this->zones.push_back(zone);
          }
          record.clear();
        }
      }
      record.push_back(line);
    }
  }

  // Insert last zone
  Zone zone(record);
  if (!zone.empty()) {
    index.insert(std::make_pair(zone.bounding_box, zones.size()));
    zones.push_back(record);
  }

  f.close();
}

void Airspace::infractions(const PointCollection &points,
                           infractions_t &infractions) const {
  std::vector<box_mapping_t> first_pass;
  geopoints_t zone_infractions;

  linestring_t flight_track(points.begin(), points.end());
  this->index.query(bgi::intersects(flight_track),
                    std::back_inserter(first_pass));

  for (const box_mapping_t &zone_index : first_pass) {
    zone_infractions = this->zones[zone_index.second].contained_points(points);
    if (!zone_infractions.empty()) {
      std::cerr << this->zones[zone_index.second].name << std::endl;
      infractions[zones[zone_index.second].name] = zone_infractions;
    }
  }
}