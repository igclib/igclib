#pragma once
#include <igclib/flight.hpp>
#include <igclib/json.hpp>
#include <igclib/task.hpp>
#include <mutex>
#include <string>

class Snapshot {
public:
  Snapshot(const Time &t);
  void add(const std::string &id, const RaceStatus &status);

  json to_json() const;
  const Time &time() const { return this->m_time; }

protected:
  Time m_time;
  std::vector<std::pair<std::string, RaceStatus>> m_status;
};

class Race {
public:
  Race(const std::string &flight_dir, const std::string &task_file);
  void save(const std::string &out) const;
  json to_json() const;
  Snapshot at(const Time &t) const;

  const Time &start() const;
  const Time &close() const;

  void _build_flight(const std::string &filename);

protected:
  Time m_first_takeoff;
  Time m_first_track_on;
  Time m_last_landing;
  Time m_last_track_off;

  std::unique_ptr<Task> m_task;
  std::vector<std::shared_ptr<RaceFlight>> m_flights;
  std::vector<std::shared_ptr<Snapshot>> m_snapshots;

  std::mutex _mutex;
};
