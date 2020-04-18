#include <fstream>
#include <igclib/geopoint.hpp>
#include <igclib/json.hpp>
#include <igclib/logging.hpp>
#include <igclib/task.hpp>
#include <igclib/time.hpp>
#include <igclib/turnpoint.hpp>
#include <memory>
#include <string>

// Creates an task from a valid JSON file, according to (soon) FFVL spec
PWCATask::PWCATask(const std::string &task_file) {
  std::ifstream f(task_file);
  json j;
  f >> j;
  throw std::runtime_error("PWCA tasks are not implemented yet");
  logging::debug({this->m_all_tp.back()->to_string()});
}