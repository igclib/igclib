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
FFVLTask::FFVLTask(const std::string &task_file) {
  std::ifstream f(task_file);
  json j;
  f >> j;

  for (auto &t : j.at("turnpoints")) {
    std::size_t radius = t.at("radius");
    double lat = t.at("lat");
    double lon = t.at("lon");
    int alt = t.at("altitude");
    std::string name = t.at("name");
    std::string desc = t.at("description");

    if (t.at("role") == "takeoff") {
      auto &&parsed = std::make_shared<Takeoff>(GeoPoint(lat, lon, alt), radius,
                                                name, desc);
      this->m_takeoff = parsed;
      this->m_all_tp.push_back(parsed);
    } else if (t.at("role") == "start") {
      int sec_from_midnight = t.at("open");
      Time &&open(sec_from_midnight);
      auto &&parsed = std::make_shared<SSS>(GeoPoint(lat, lon, alt), radius,
                                            name, desc, open);
      this->m_sss = parsed;
      this->m_all_tp.push_back(parsed);
    } else if (t.at("role") == "turnpoint") {
      auto &&parsed = std::make_shared<Turnpoint>(GeoPoint(lat, lon, alt),
                                                  radius, name, desc);
      this->m_all_tp.push_back(parsed);
      this->m_tp.push_back(parsed);
    } else if (t.at("role") == "ess") {
      auto &&parsed =
          std::make_shared<ESS>(GeoPoint(lat, lon, alt), radius, name, desc);
      this->m_ess = parsed;
      this->m_all_tp.push_back(parsed);
      this->m_tp.push_back(parsed);
    } else if (t.at("role") == "goal") {
      bool line = t.at("finish") != "cylinder";
      int sec_from_midnight = t.at("close");
      Time &&close(sec_from_midnight);
      auto &&parsed = std::make_shared<Goal>(GeoPoint(lat, lon, alt), radius,
                                             name, desc, close, line);
      this->m_goal = parsed;
      this->m_all_tp.push_back(parsed);
      this->m_tp.push_back(parsed);
    }
    logging::debug({"[ TASK ]", this->m_all_tp.back()->to_string()});
  }
  this->flatten();
}