#pragma once

#include <igclib/airspace.hpp>
#include <igclib/geopoint.hpp>
#include <igclib/xcscore.hpp>
#include <map>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

class Flight {
  typedef std::vector<GeoPoint> geopoints_t;
  typedef std::map<std::string, geopoints_t> infractions_t;

public:
  Flight(const std::string &flight_file);
  void save(const std::string &out) const;
  void validate(const Airspace &airspace);
  nlohmann::json serialize() const;
  bool agl_validable;

private:
  void process_H_record(const std::string &record);
  void process_B_record(const std::string &record);
  void compute_score();

  int time_zone_offset = 0;
  std::string pilot_name = "Unknown pilot";
  XCScore score;
  PointCollection points;
  infractions_t infractions;
};