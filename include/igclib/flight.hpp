#pragma once

#include <igclib/airspace.hpp>
#include <igclib/geopoint.hpp>
#include <igclib/xcscore.hpp>
#include <map>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

class Flight {

public:
  Flight(const std::string &flight_file);
  void save(const std::string &out) const;
  void validate(const Airspace &airspace);
  json serialize() const;

private:
  void process_H_record(const std::string &record);
  void process_B_record(const std::string &record);
  void compute_score();

  XCScore score;
  std::string pilot_name = "Unknown pilot";
  int time_zone_offset = 0;
  PointCollection points;
  std::map<std::string, std::vector<GeoPoint>> infractions;
};