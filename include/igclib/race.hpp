#pragma once
#include <igclib/flight.hpp>
#include <igclib/task.hpp>
#include <string>

class Race {
public:
  Race(const std::string &flight_dir, const std::string &task_file);
  void save(const std::string &out) const;

protected:
  std::unique_ptr<Task> m_task;
  std::vector<std::unique_ptr<RaceFlight>> m_flights;
};