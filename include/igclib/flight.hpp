#pragma once

#include "igclib/airspace.hpp"
#include "igclib/geo.hpp"
#include "igclib/time.hpp"
#include <string>
#include <unordered_map>
#include <vector>

typedef boost::geometry::model::linestring<GeoPoint> linestring_t;

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
  std::unordered_map<Time, GeoPoint> points;
  linestring_t linestring;
  std::vector<GeoPoint> infractions;
};