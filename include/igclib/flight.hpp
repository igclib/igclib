#pragma once

#include <igclib/airspace.hpp>
#include <igclib/geopoint.hpp>
#include <igclib/xcinfo.hpp>
#include <map>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

class CandidateTree;
class ObjectiveFunction;
class BoundingFunction;

class Flight {
  typedef std::vector<GeoPoint> geopoints_t;
  typedef std::map<std::string, geopoints_t> infractions_t;

public:
  Flight(const std::string &igc_file);
  void save(const std::string &out) const;
  void validate(const Airspace &airspace);
  nlohmann::json serialize() const;
  void compute_score();
  geopoints_t bbox(std::pair<std::size_t, std::size_t> pair) const {
    return this->points.bbox(pair.first, pair.second);
  };
  double max_diagonal(std::pair<int, int> pair) const {
    return this->points.max_diagonal(pair.first, pair.second);
  };
  std::size_t size() const { return this->points.size(); }

private:
  void process_H_record(const std::string &record);
  void process_B_record(const std::string &record);
  double heuristic_score();

  int time_zone_offset = 0;
  XCInfo xcinfo;
  std::string pilot_name = "Unknown pilot";
  PointCollection points;
  infractions_t infractions;
};
