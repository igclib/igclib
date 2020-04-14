#include <dlib/optimization.h>
#include <fstream>
#include <igclib/logging.hpp>
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
  // std::vector<double> theta(this->m_task->n_turnpoints(), 0);
  dlib::matrix<double, 0, 1> theta(this->m_task->n_turnpoints(), 0);
  const auto &c = this->m_task->centers();
  const auto &r = this->m_task->radii();
  auto f = [c, r](const dlib::matrix<double, 0, 1> &t) {
    return Task::routedist(t, c, r);
  };
  // auto f = [this](int a, bool b) -> double {};
  dlib::find_max_using_approximate_derivatives(
      dlib::bfgs_search_strategy(), // Use BFGS search algorithm
      dlib::objective_delta_stop_strategy(1e-7), f, theta, -1);
  logging::debug(
      {"Optimized distance", std::to_string(this->routedist(theta, c, r))});
}

double Task::routedist(const dlib::matrix<double, 0, 1> &theta,
                       const std::vector<GeoPoint> &centers,
                       const std::vector<std::size_t> &radii) {
  double distance = 0;
  GeoPoint last_proj = centers.front().project(radii.front(), theta(0));
  GeoPoint next_proj;
  for (std::size_t i = 1; i < centers.size(); i++) {
    next_proj = centers.at(i).project(radii.at(i), theta(i));
    distance += last_proj.distance(next_proj);
    last_proj = next_proj;
  }
  return distance;
}

void Task::save(const std::string &out) const { (void)out; }

void TaskImpl::flatten() {
  for (const auto &tp : this->m_all_tp) {
    this->m_centers.push_back(tp->center());
    this->m_radii.push_back(tp->radius());
  }
}