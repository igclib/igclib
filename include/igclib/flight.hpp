#pragma once

#include <igclib/airspace.hpp>
#include <igclib/geopoint.hpp>
#include <igclib/json.hpp>
#include <igclib/xcinfo.hpp>
#include <map>
#include <memory>
#include <string>
#include <vector>

class Flight {
  typedef std::vector<std::pair<std::shared_ptr<Zone>, PointCollection>>
      infractions_t;

public:
  Flight(const std::string &igc_file);
  void save(const std::string &out) const;
  void validate(const Airspace &airspace);
  void compute_score(/*TODO add rules ?*/);
  json to_json() const;
  std::array<GeoPoint, 4> bbox(std::pair<std::size_t, std::size_t> pair) const {
    return this->m_points.bbox(pair.first, pair.second);
  };
  double max_diagonal(std::pair<int, int> pair) const {
    return this->m_points.max_diagonal(pair.first, pair.second);
  };
  std::size_t size() const { return this->m_points.size(); }

  const std::shared_ptr<GeoPoint> at(std::size_t index) const;
  const std::shared_ptr<GeoPoint> at(const Time &time) const;

private:
  void process_H_record(const std::string &record);
  void process_B_record(const std::string &record);
  double heuristic_free() const;
  template <class T> double optimize_xc(double lower_bound);
  Time time_zone_offset;
  std::string pilot_name = "Unknown pilot";
  XCInfo xcinfo;
  PointCollection m_points;
  infractions_t m_infractions;
};
