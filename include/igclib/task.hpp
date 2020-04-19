#pragma once

#include <igclib/geopoint.hpp>
#include <igclib/json.hpp>
#include <igclib/route.hpp>
#include <igclib/turnpoint.hpp>
#include <memory>
#include <string>
#include <vector>

using Taskformat = enum TaskFormat { XCTRACK, PWCA, FFVL, UNKOWN };

class TaskImpl {
  friend class Task;

public:
  TaskImpl() = default;

  json to_json() const;

protected:
  // centers and radii used by optimizer
  std::deque<GeoPoint> m_centers;
  std::deque<std::size_t> m_radii;

  // task elements
  std::shared_ptr<Takeoff> m_takeoff;
  std::shared_ptr<SSS> m_sss;
  std::shared_ptr<ESS> m_ess;
  std::shared_ptr<Goal> m_goal;

  // all tp
  std::vector<std::shared_ptr<Turnpoint>> m_all_tp;
  std::vector<std::shared_ptr<Turnpoint>> m_tp;
};

class FFVLTask : public TaskImpl {
public:
  FFVLTask() = delete;
  FFVLTask(const std::string &task_file);
};

class XCTask : public TaskImpl {
public:
  XCTask() = delete;
  XCTask(const std::string &task_file);
};

class PWCATask : public TaskImpl {
public:
  PWCATask() = delete;
  PWCATask(const std::string &task_file);
};

class Task {
public:
  Task(const std::string &task_file);
  void save(const std::string &out) const;
  json to_json() const;

  std::size_t n_turnpoints() const { return this->m_task->m_all_tp.size(); }
  double length() const { return this->m_route.optimal_distance(); }

  const std::deque<GeoPoint> &centers() const {
    return this->m_task->m_centers;
  };
  const std::deque<std::size_t> &radii() const {
    return this->m_task->m_radii;
  };
  std::shared_ptr<Takeoff> takeoff() const { return this->m_task->m_takeoff; }
  std::shared_ptr<SSS> sss() const { return this->m_task->m_sss; }
  std::shared_ptr<ESS> ess() const { return this->m_task->m_ess; }
  std::shared_ptr<Goal> goal() const { return this->m_task->m_goal; }
  auto begin() const { return this->m_task->m_all_tp.begin(); }
  auto end() const { return this->m_task->m_all_tp.end(); }

  const Time &start() const;
  const Time &close() const;

protected:
  void identify(const std::string &task_file);

  std::string m_filename;
  Route m_route;
  std::shared_ptr<TaskImpl> m_task;
  TaskFormat m_format;
  std::vector<Time> m_turnpoint_tags;
};
