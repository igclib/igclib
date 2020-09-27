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
  std::size_t size() const { return m_points.size(); }
  const std::string &pilot() const { return m_pilot_name; }
  const std::string &id() const { return m_file_name; }
  const std::shared_ptr<GeoPoint> at(std::size_t index) const {
    return m_points.at(index);
  }
  const std::shared_ptr<GeoPoint> at(const Time &time) const {
    return m_points.at(time);
  }

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
  std::shared_ptr<GeoPoint> _position;
  FlightStatus _status;
  double _goal_distance;
};

class RaceFlight : public Flight {
public:
  RaceFlight(const std::string &igc_file, const Task &task);
  RaceStatus at(const Time &t) const;

  const Time takeoff_time() const { return m_takeoff; }
  const Time landing_time() const { return m_landing; }
  const Time race_time() const { return _race_time; }

  json to_json() const;

protected:
  void validate_race(const Task &task);
  void validate_elapsed(const Task &task);

  Time m_takeoff;
  Time m_landing;
  Time _track_on;
  Time _track_off;

  std::map<Time, RaceStatus> _status;
  std::vector<Time> _tag_times;
  Time _race_time;
};