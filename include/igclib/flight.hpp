#pragma once

#include "igclib/airspace.hpp"
#include "igclib/geopoint.hpp"
#include <string>
#include <vector>

class Flight {

public:
  Flight(const std::string &flight_file);
  void to_JSON() const;
  void validate(const Airspace &airspace);

  std::string pilot_name = "Unknown pilot";

private:
  void process_H_record(const std::string &record);
  void process_B_record(const std::string &record);

  int time_zone_offset = 0;
  PointCollection points;
  std::vector<GeoPoint> infractions;
};