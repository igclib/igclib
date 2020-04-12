#pragma once

#include <igclib/airspace.hpp>
#include <igclib/geopoint.hpp>
#include <igclib/xcinfo.hpp>
#include <map>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

class Flight {
  typedef std::vector<GeoPoint> geopoints_t;

public:
  Flight(const std::string &igc_file);
  void save(const std::string &out) const;
  void validate(const Airspace &airspace);
  void compute_score(/*TODO add rules ?*/);
  nlohmann::json serialize() const;
  geopoints_t bbox(std::pair<std::size_t, std::size_t> pair) const {
    return this->points.bbox(pair.first, pair.second);
  };
  double max_diagonal(std::pair<int, int> pair) const {
    return this->points.max_diagonal(pair.first, pair.second);
  };
  std::size_t size() const { return this->points.size(); }
  const GeoPoint &at(std::size_t index) const { return this->points.at(index); }

private:
  void process_H_record(const std::string &record);
  void process_B_record(const std::string &record);
  double heuristic_free() const;
  template <class T> double optimize_xc(double lower_bound);
  int time_zone_offset = 0;
  XCInfo xcinfo;
  std::string pilot_name = "Unknown pilot";
  PointCollection points;
  std::map<std::shared_ptr<Zone>, std::vector<GeoPoint>> infractions;
};
