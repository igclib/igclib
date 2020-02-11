#pragma once

#include "igclib/geo.hpp"
#include "igclib/time.hpp"
#include <map>
#include <string>

class Flight {

public:
  Flight(const std::string &filename);
  void to_JSON() const;

private:
  void process_H_record(const std::string &record);
  void process_B_record(const std::string &record);

  std::map<Time, GeoPoint> points;
  std::string pilot_name = "Unknown pilot";
  int time_zone_offset = 0;
};