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

protected:
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
  void identify(const std::string &task_file);
  void save(const std::string &out) const;

protected:
  std::vector<GeoPoint> optimized_route;
  std::vector<std::size_t> optimized_legs;
  std::vector<std::size_t> center_legs;
  std::shared_ptr<TaskImpl> task;
  TaskFormat format = TaskFormat::UNKOWN;
};
