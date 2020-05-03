#include <boost/algorithm/string.hpp>
#include <fstream>
#include <igclib/airspace.hpp>
#include <igclib/geopoint.hpp>
#include <igclib/logging.hpp>
#include <igclib/time.hpp>
#include <igclib/util.hpp>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

Airspace::Airspace(const std::string &airspace_file) : _needs_agl_checking(0) {
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
        _update_index(record);
      }
      record.push_back(line);
    }
  }

  // Insert last zone
  _update_index(record);
}

// adds the zone built from the record to the index and clears the record
void Airspace::_update_index(std::vector<std::string> &record) {
  if (!record.empty()) {
    Zone zone(record);
    // zone might be empty if no geometry could be parsed from the record
    // in this case, do not add it to the index, but clear the record
    // anyway
    if (!zone.empty()) {
      _index.insert(
          std::make_pair(zone.bounding_box, std::make_shared<Zone>(zone)));
    }
    if (zone.needs_agl_checking()) {
      ++_needs_agl_checking;
    }
    record.clear();
  }
}

void Airspace::infractions(const PointCollection &flight,
                           infractions_t &infractions, bool with_agl) const {

  // TODO do in_altitude_range predicate to discard useless zones faster
  // https://www.boost.org/doc/libs/1_66_0/libs/geometry/doc/html/geometry/spatial_indexes/queries.html

  std::vector<box_mapping_t> first_pass;

  bg::model::linestring<GeoPoint> flight_track;
  for (const auto p : flight.geopoints()) {
    flight_track.push_back(*p);
  }

  _index.query(bgi::intersects(flight_track), /* &&
                         bgi::satisfies([](auto const &v) {
                           (void)v;
                           return true;
                         }),*/
               std::back_inserter(first_pass));

  std::vector<std::thread> jobs;
  std::mutex mutex;
  for (const box_mapping_t &possible_infraction : first_pass) {
    jobs.emplace_back(
        //[&](std::mutex &mut, infractions_t &infra) {
        std::thread([&, possible_infraction] {
          auto zone_infractions =
              possible_infraction.second->contained_points(flight, with_agl);
          if (!zone_infractions.geopoints().empty()) {
            std::lock_guard<std::mutex> lock(mutex);
            logging::debug({"infraction", possible_infraction.second->m_name});
            infractions.emplace_back(possible_infraction.second,
                                     zone_infractions);
          }
        }));
    // std::ref(mutex), std::ref(infractions));
  }
  for (auto &j : jobs) {
    j.join();
  }
}