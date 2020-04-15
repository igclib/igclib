#include <fstream>
#include <igclib/json.hpp>
#include <igclib/logging.hpp>
#include <igclib/route.hpp>
#include <igclib/task.hpp>
#include <iostream>
#include <string>

Task::Task(const std::string &task_file) {
  this->identify(task_file);
  switch (this->m_format) {
  case TaskFormat::XCTRACK:
    logging::debug({"File", task_file, "identified as XCTRACK task format"});
    this->m_task = std::make_shared<XCTask>(task_file);
    break;
  case TaskFormat::PWCA:
    logging::debug({"File", task_file, "identified as PWCA task format"});
    this->m_task = std::make_shared<PWCATask>(task_file);
    break;
  case TaskFormat::FFVL:
    logging::debug({"File", task_file, "identified as FFVL task format"});
    this->m_task = std::make_shared<FFVLTask>(task_file);
    break;
  case TaskFormat::UNKOWN:
    throw std::runtime_error("Unknown file format");
    break;
  default:
    throw std::runtime_error(
        "You're not supposed to be here [task identification]");
    break;
  }

  this->m_route = Route(this->m_task->centers().front(),
                        this->m_task->centers(), this->m_task->radii());
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
  } catch (const json::parse_error &e) {
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

void TaskImpl::flatten() {
  for (const auto &tp : this->m_all_tp) {
    this->m_centers.push_back(tp->center());
    this->m_radii.push_back(tp->radius());
  }
}

json TaskImpl::to_json() const {
  json j;
  j["takeoff"] = this->m_takeoff->to_json();
  j["sss"] = this->m_sss->to_json();
  j["ess"] = this->m_ess->to_json();
  j["goal"] = this->m_goal->to_json();
  for (const auto &tp : this->m_all_tp) {
    j["turnpoints"].push_back(tp->to_json());
  }
  return j;
}