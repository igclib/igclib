#include <fstream>
#include <igclib/logging.hpp>
#include <igclib/task.hpp>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

Task::Task(const std::string &task_file) {
  this->identify(task_file);
  switch (this->format) {
  case TaskFormat::XCTRACK:
    logging::debug({"File", task_file, "identified as XCTRACK task format"});
    this->task = std::make_shared<XCTask>(task_file);
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
    throw std::runtime_error("You're not supposed to be here");
    break;
  }
}

// For now all supported task formats are JSON files. If this was to change, it
// would be wise to implement a TaskIdentifier class with the different cases.
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

  // XCTRACK identifiers
  if (j.contains("taskType") && j.contains("earthcore")) {
    this->format = TaskFormat::XCTRACK;
    matching_formats++;
  }

  // FFVL identifiers
  if (j.contains("source")) {
    this->format = TaskFormat::FFVL;
    matching_formats++;
  }

  if (matching_formats > 1) {
    throw std::runtime_error("File matches multiple possible formats");
  } else if (matching_formats < 1) {
    throw std::runtime_error("File doesn't match any known format");
  }
}

void Task::save(const std::string &out) const { (void)out; }