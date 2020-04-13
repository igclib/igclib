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
      // normalize all lines to uppercase for parsing
      boost::to_upper(line);
      // zones are delimited by AC instructions
      if (line.substr(0, 2) == "AC") {
        if (!record.empty()) {
          this->update_index(record);
        }
      }
      record.push_back(line);
    }
  }

  // Insert last zone
  this->update_index(record);
}

void Airspace::update_index(std::vector<std::string> &record) {
  Zone zone(record);
  // zone might be empty if no geometry could be parsed from the record
  // in this case, do not add it to the index, but clear the record
  // anyway
  if (!zone.empty()) {
    this->index.insert(
        std::make_pair(zone.bounding_box, std::make_shared<Zone>(zone)));
  }
  if (zone.needs_agl_checking()) {
    this->needs_agl_checking++;
  }
  record.clear();
}

void Airspace::infractions(const PointCollection &points,
                           infractions_t &infractions, bool with_agl) const {

  // TODO do in_altitude_range predicate to discard useless zones faster
  int useless = 0;
  // https://www.boost.org/doc/libs/1_66_0/libs/geometry/doc/html/geometry/spatial_indexes/queries.html

  std::vector<box_mapping_t> first_pass;
  std::vector<GeoPoint> zone_infractions;

  bg::model::linestring<GeoPoint> flight_track(points.begin(), points.end());
  this->index.query(bgi::intersects(flight_track) &&
                        bgi::satisfies([](auto const &v) { return true; }),
                    std::back_inserter(first_pass));

  for (const box_mapping_t &possible_infraction : first_pass) {
    zone_infractions =
        possible_infraction.second->contained_points(points, with_agl);
    if (!zone_infractions.empty()) {
      std::cerr << "INFRACTION " << possible_infraction.second->m_name
                << std::endl;
      infractions[possible_infraction.second] = zone_infractions;
    } else {
      useless++;
    }
  }
  // std::cerr << "USELESS CHECKING : " << useless << std::endl;
}