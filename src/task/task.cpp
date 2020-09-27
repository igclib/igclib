#include <boost/chrono/process_cpu_clocks.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <igclib/json.hpp>
#include <igclib/logging.hpp>
#include <igclib/route.hpp>
#include <igclib/task.hpp>
#include <iostream>
#include <string>

Task::Task(const std::string &task_file) : m_format(TaskFormat::UNKOWN) {
  this->identify(task_file);
  this->m_filename = boost::filesystem::path(task_file).filename().string();
  switch (this->m_format) {
  case TaskFormat::XCTRACK:
    logging::debug({"[ TASK ]", this->m_filename, "identified as XCTRACK"});
    this->m_task = std::make_shared<XCTask>(task_file);
    break;
  case TaskFormat::FFVL:
    logging::debug({"[ TASK ]", this->m_filename, "identified as FFVL"});
    this->m_task = std::make_shared<FFVLTask>(task_file);
    break;
  case TaskFormat::UNKOWN:
    throw std::runtime_error("[ TASK ] unknown file format");
    break;
  default:
    throw std::runtime_error("[ TASK ] you're not supposed to be here");
    break;
  }

  // add all turnpoints except the first one
  auto tp = this->end();
  while (tp-- > this->begin() + 1) {
    this->m_task->m_centers.push_front(tp->get()->center());
    this->m_task->m_radii.push_front(tp->get()->radius());
  }

  // compute the route with first turnpoint as initial position
  this->m_route = Route(tp->get()->center(), this->centers(), this->radii());

  // finally add the first turnpoint
  this->m_task->m_centers.push_front(tp->get()->center());
  this->m_task->m_radii.push_front(tp->get()->radius());

  logging::debug({"[ TASK ]",
                  std::to_string(this->m_route.optimal_distance() / 1000),
                  "km"});
}

// Identifies the format of the task file from known providers, based on
// distinctive traits. For now all supported task formats are JSON files. If
// this was to change, it would be wise to implement a TaskIdentifier class
// with the different cases.
void Task::identify(const std::string &task_file) {
  std::size_t matching_formats = 0;
  std::ifstream f(task_file);
  json j;
  try {
    f >> j;
  } catch (const json::parse_error &e) {
    logging::debug({"[ TASK ]", e.what()});
    throw std::runtime_error(task_file + " is not valid JSON");
  }

  // XCTRACK traits
  if (j.contains("taskType") &&
      (j.contains("earthcore") || j.contains("earthModel"))) {
    this->m_format = TaskFormat::XCTRACK;
    matching_formats++;
  }

  // FFVL traits
  if (j.contains("source")) {
    this->m_format = TaskFormat::FFVL;
    matching_formats++;
  }

  if (matching_formats > 1) {
    throw std::runtime_error("file matches multiple possible formats");
  } else if (matching_formats < 1) {
    throw std::runtime_error("file doesn't match any known format");
  }
}

json Task::to_json() const {
  json j;
  j["task"] = this->m_task->to_json();
  j["route"] = this->m_route.to_json();
  return j;
}

void Task::save(const std::string &out) const {
  json j = this->to_json();
  if (out == "-" || out.empty()) {
    std::cout << j.dump(4) << std::endl;
  } else {
    std::ofstream f;
    f.open(out);
    if (!f.is_open()) {
      const std::string error = "Could not open file '" + out + "'";
      throw std::runtime_error(error);
    }
    f << j.dump(4);
  }
}

json TaskImpl::to_json() const {
  json j;
  if (this->m_takeoff) {
    j["takeoff"] = this->m_takeoff->to_json();
  }
  j["sss"] = this->m_sss->to_json();
  j["ess"] = this->m_ess->to_json();
  if (this->m_goal) {
    j["goal"] = this->m_goal->to_json();
  }
  for (const auto tp : this->m_all_tp) {
    j["turnpoints"].push_back(tp->to_json());
  }
  return j;
}

bool Task::has_close_time() const {
  if (this->m_task->m_goal) {
    return true;
  }
  return false;
}

const Time Task::start() const { return this->m_task->m_sss->open(); }
const Time Task::close() const { return this->m_task->m_goal->close(); }