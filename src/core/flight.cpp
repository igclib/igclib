#include "igclib/flight.hpp"
#include "igclib/time.hpp"
#include "igclib/util.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

Flight::Flight(const std::string &filename) {
  std::ifstream f;
  f.open(filename);
  if (!f.is_open()) {
    const std::string error = "Could not open file '" + filename + "'";
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

void Flight::process_B_record(const std::string &record){
  Time t(record.substr(1, 6), this->time_zone_offset);
  GeoPoint p(record.substr(7));
  this->points.emplace(t, p);
}

void Flight::to_JSON() const{
  std::cerr << this->points.size() << " points in map" << std::endl;
}