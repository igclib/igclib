#pragma once

#include <igclib/geopoint.hpp>
#include <igclib/json.hpp>
#include <igclib/route.hpp>
#include <igclib/turnpoint.hpp>
#include <memory>
#include <string>
#include <vector>

typedef enum TaskFormat { XCTRACK, PWCA, FFVL, UNKOWN } TaskFormat;

class TaskImpl {
public:
  TaskImpl(){};

  std::size_t n_turnpoints() const { return this->m_all_tp.size(); }
  const std::vector<GeoPoint> &centers() const { return this->m_centers; };
  const std::vector<std::size_t> &radii() const { return this->m_radii; };
  json to_json() const;

protected:
  // flatten generates the centers and radii vectors of all turnpoints
  void flatten();
  std::vector<GeoPoint> m_centers;
  std::vector<std::size_t> m_radii;

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
  FFVLTask(const std::string &task_file);
};

class XCTask : public TaskImpl {
public:
  XCTask(const std::string &task_file);
};

class PWCATask : public TaskImpl {
public:
  PWCATask(const std::string &task_file);
};

class Task {
public:
  Task(const std::string &task_file);
  void save(const std::string &out) const;
  json to_json() const;

protected:
  void identify(const std::string &task_file);
  void compute_optimized_route();

  Route m_route;
  std::shared_ptr<TaskImpl> m_task;
  TaskFormat m_format = TaskFormat::UNKOWN;
};
