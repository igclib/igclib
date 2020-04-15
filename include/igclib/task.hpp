#pragma once

#include <igclib/geopoint.hpp>
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

protected:
  void flatten();

  std::vector<GeoPoint> m_centers;
  std::vector<std::size_t> m_radii;
  std::shared_ptr<Takeoff> m_takeoff;
  std::shared_ptr<SSS> m_sss;
  std::vector<std::shared_ptr<Turnpoint>> m_all_tp;
  std::vector<std::shared_ptr<Turnpoint>> m_tp;
  std::shared_ptr<ESS> m_ess;
  std::shared_ptr<Goal> m_goal;
};

class FFVLTask : public TaskImpl {};

class XCTask : public TaskImpl {
public:
  XCTask(const std::string &task_file);
};

class Task {
public:
  Task(const std::string &task_file);
  void save(const std::string &out) const;

protected:
  void identify(const std::string &task_file);
  void compute_optimized_route();

  std::vector<GeoPoint> m_optimized_route;
  std::vector<std::size_t> m_optimized_legs;
  std::vector<std::size_t> m_center_legs;
  std::shared_ptr<TaskImpl> m_task;
  TaskFormat m_format = TaskFormat::UNKOWN;
};
