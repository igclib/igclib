#pragma once

#include <igclib/airspace.hpp>
#include <igclib/geopoint.hpp>
#include <igclib/json.hpp>
#include <igclib/task.hpp>
#include <igclib/xcinfo.hpp>
#include <map>
#include <memory>
#include <string>
#include <vector>

class Flight {
  typedef std::vector<std::pair<std::shared_ptr<Zone>, PointCollection>>
      infractions_t;
  using sizepair = std::pair<std::size_t, std::size_t>;

public:
  Flight(const std::string &igc_file);
  void save(const std::string &out) const;
  void validate(const Airspace &airspace);
  void compute_score(/*TODO add rules ?*/);
  json to_json() const;
  std::array<GeoPoint, 4> bbox(sizepair pair) const;
  double max_diagonal(sizepair p) const;

  // accessors
  std::size_t size() const;
  const std::string &pilot() const;
  const std::string &id() const;
  const std::shared_ptr<GeoPoint> at(std::size_t index) const;
  const std::shared_ptr<GeoPoint> at(const Time &time) const;

protected:
  void process_H_record(const std::string &record);
  void process_B_record(const std::string &record);
  double heuristic_free() const;
  template <class T> double optimize_xc(double lower_bound);

  std::string m_pilot_name;
  std::string m_file_name;
  IGCTimeOffset m_timezone_offset;
  XCInfo m_xcinfo;
  PointCollection m_points;
  infractions_t m_infractions;
};

/* RACEFLIGHT */

using FlightStatus = enum FlightStatus {
  ON_TAKEOFF,
  IN_FLIGHT,
  IN_GOAL,
  LANDED,
  UNKNOWN,
};

class RaceStatus {

public:
  RaceStatus() = default;
  RaceStatus(std::shared_ptr<GeoPoint> pos, double dist_to_goal,
             const FlightStatus &status);
  json to_json() const;

protected:
  std::shared_ptr<GeoPoint> m_position;
  FlightStatus m_status;
  double m_goal_distance;
};

class RaceFlight : public Flight {
public:
  RaceFlight(const std::string &igc_file, const Task &task);
  RaceStatus at(const Time &t) const;

  const Time &takeoff_time() const;
  const Time &landing_time() const;

protected:
  void validate(const Task &task);

  Time m_takeoff;
  Time m_landing;
  Time m_track_on;
  Time m_track_off;

  std::map<Time, RaceStatus> m_status;
  std::vector<Time> m_tag_times;
};