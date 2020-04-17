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
  std::array<GeoPoint, 4> bbox(std::pair<std::size_t, std::size_t> pair) const;
  double max_diagonal(std::pair<std::size_t, std::size_t> p) const;

  // accessors
  std::size_t size() const;
  const std::string &pilot() const;
  const std::shared_ptr<GeoPoint> at(std::size_t index) const;
  const std::shared_ptr<GeoPoint> at(const Time &time) const;

protected:
  void process_H_record(const std::string &record);
  void process_B_record(const std::string &record);
  double heuristic_free() const;
  template <class T> double optimize_xc(double lower_bound);

  std::string m_pilot_name;
  std::string file_name;
  Time m_timezone_offset;
  XCInfo m_xcinfo;
  PointCollection m_points;
  infractions_t m_infractions;
};

class RaceFlight : public Flight {
public:
  RaceFlight(const std::string &igc_file);

protected:
};