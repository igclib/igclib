#pragma once

#include "igclib/airspace.hpp"
#include "igclib/geopoint.hpp"
#include <map>
#include <string>
#include <vector>

class Flight {

public:
  Flight(const std::string &flight_file);
  void to_JSON(const std::string &out) const;
  void validate(const Airspace &airspace);

  const std::string &pilot() const { return this->pilot_name; };

private:
  std::string pilot_name = "Unknown pilot";
  void process_H_record(const std::string &record);
  void process_B_record(const std::string &record);

  int time_zone_offset = 0;
  PointCollection points;
  std::map<std::string, std::vector<GeoPoint>> infractions;
};