#include "igclib/flight.hpp"
#include "igclib/airspace.hpp"
#include "igclib/time.hpp"
#include "igclib/util.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

Flight::Flight(const std::string &flight_file) {
  // read and parse igc file
  std::ifstream f;
  f.open(flight_file);
  if (!f.is_open()) {
    const std::string error = "Could not open file '" + flight_file + "'";
    throw std::runtime_error(error);
  }

  std::string line;
  while (std::getline(f, line)) {
    switch (line[0]) {
    // header records
    case 'H':
      process_H_record(line);
      break;
    // fix records
    case 'B':
      process_B_record(line);
    default:
      break;
    }
  }

  // compute geometry
  double min_lat = 90.0;
  double max_lat = -90.0;
  double min_lon = 180.0;
  double max_lon = -180.0;
  double min_alt = 10000; // this is arbitrary
  double max_alt = 0;
  for (auto &it : this->points) {
    if (it.second.lat < min_lat) {
      min_lat = it.second.lat;
    }
    if (it.second.lat > max_lat) {
      max_lat = it.second.lat;
    }
    if (it.second.lon < min_lon) {
      min_lon = it.second.lon;
    }
    if (it.second.lon > max_lon) {
      max_lon = it.second.lon;
    }
    if (it.second.alt < min_alt) {
      min_alt = it.second.alt;
    }
    if (it.second.alt > max_alt) {
      max_alt = it.second.alt;
    }
  }

#ifndef NDEBUG
  std::cerr << this->points.size() << " points in flight" << std::endl;
  std::cerr << min_lat << " : " << max_lat << " - " << min_lon << " : "
            << max_lon << " - " << min_alt << " : " << max_alt << std::endl;
#endif

  f.close();
}

void Flight::process_H_record(const std::string &record) {
  const std::string record_code = record.substr(2, 3);
  if (record_code == "PLT") {
    size_t delim = record.find(':') + 1;
    this->pilot_name = record.substr(delim);
    util::trim(this->pilot_name);
  } else if (record_code == "TZO") {
    size_t delim = record.find(':') + 1;
    this->time_zone_offset = std::stoi(record.substr(delim));
  }
}

void Flight::process_B_record(const std::string &record) {
  Time t(record.substr(1, 6), this->time_zone_offset);
  GeoPoint p(record.substr(7));
  this->points.emplace(t, p);
}

void Flight::to_JSON() const {}

void Flight::validate(const Airspace &airspace) {}