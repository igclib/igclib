#pragma once
#include <igclib/flight.hpp>
#include <igclib/json.hpp>
#include <igclib/task.hpp>
#include <mutex>
#include <string>

class Snapshot {
public:
  Snapshot(const Time &t) : _time(t) {}
  void add(const std::string &id, const RaceStatus &status);

  json to_json() const;
  const Time &time() const { return _time; }

protected:
  Time _time;
  std::vector<std::pair<std::string, RaceStatus>> _status;
};

class Race {
public:
  Race(const std::string &flight_dir, const std::string &task_file);
  void save(const std::string &out) const;
  json to_json() const;
  Snapshot at(const Time &t) const;

  const Time &start() const;
  const Time &close() const;

protected:
  Time _first_takeoff;
  Time _first_track_on;
  Time _last_landing;
  Time _last_track_off;

  Task _task;
  std::vector<std::shared_ptr<RaceFlight>> _flights;
  std::vector<std::shared_ptr<Snapshot>> _snapshots;
};
