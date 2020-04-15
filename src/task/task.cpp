#include <cppoptlib/solver/bfgssolver.h>
#include <fstream>
#include <igclib/logging.hpp>
#include <igclib/routedist.hpp>
#include <igclib/task.hpp>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

Task::Task(const std::string &task_file) {
  this->identify(task_file);
  switch (this->m_format) {
  case TaskFormat::XCTRACK:
    logging::debug({"File", task_file, "identified as XCTRACK task format"});
    this->m_task = std::make_shared<XCTask>(task_file);
    break;
  case TaskFormat::PWCA:
    logging::debug({"File", task_file, "identified as PWCA task format"});
    // this->task = std::make_shared<XCTask>(task_file);
    break;
  case TaskFormat::FFVL:
    logging::debug({"File", task_file, "identified as FFVL task format"});
    // this->task = std::make_shared<XCTask>(task_file);
    break;
  case TaskFormat::UNKOWN:
    throw std::runtime_error("Unknown file format");
    break;
  default:
    throw std::runtime_error(
        "You're not supposed to be here [task identification]");
    break;
  }

  this->compute_optimized_route();
}

// Identifies the format of the task file from known providers, based on
// distinctive traits. For now all supported task formats are JSON files. If
// this was to change, it would be wise to implement a TaskIdentifier class with
// the different cases.
void Task::identify(const std::string &task_file) {
  std::size_t matching_formats = 0;
  std::ifstream f(task_file);
  json j;
  try {
    f >> j;
  } catch (const nlohmann::detail::parse_error &e) {
    logging::debug({e.what()});
    throw std::runtime_error(task_file + " is not valid JSON");
  }

  // XCTRACK traits
  if (j.contains("taskType") && j.contains("earthcore")) {
    this->m_format = TaskFormat::XCTRACK;
    matching_formats++;
  }

  // FFVL traits
  if (j.contains("source")) {
    this->m_format = TaskFormat::FFVL;
    matching_formats++;
  }

  if (matching_formats > 1) {
    throw std::runtime_error("File matches multiple possible formats");
  } else if (matching_formats < 1) {
    throw std::runtime_error("File doesn't match any known format");
  }
}

void Task::compute_optimized_route() {
  RouteDist r(this->m_task->centers(), this->m_task->radii());
  Eigen::VectorXd theta(this->m_task->n_turnpoints());
  for (std::size_t i = 0; i < this->m_task->n_turnpoints(); i++) {
    theta(i, 0) = 90;
  }
  cppoptlib::BfgsSolver<RouteDist> solver;
  solver.minimize(r, theta);
  double opt_distance = r(theta);
  logging::debug({"Optimized distance", std::to_string(opt_distance)});
}

void Task::save(const std::string &out) const { (void)out; }

void TaskImpl::flatten() {
  for (const auto &tp : this->m_all_tp) {
    this->m_centers.push_back(tp->center());
    this->m_radii.push_back(tp->radius());
  }
}