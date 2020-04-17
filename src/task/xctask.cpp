#include <fstream>
#include <igclib/geopoint.hpp>
#include <igclib/json.hpp>
#include <igclib/logging.hpp>
#include <igclib/task.hpp>
#include <igclib/time.hpp>
#include <igclib/turnpoint.hpp>

// Creates an task from a valid JSON file, according to XCTrack spec
// https://xctrack.org/Competition_Interfaces.html
XCTask::XCTask(const std::string &task_file) {
  std::ifstream f(task_file);
  json j;
  f >> j;

  for (auto &t : j.at("turnpoints")) {
    std::size_t radius = t.at("radius");
    double lat = t.at("waypoint").at("lat");
    double lon = t.at("waypoint").at("lon");
    int alt = t.at("waypoint").at("altSmoothed");
    std::string name = t.at("waypoint").at("name");
    std::string desc = t.at("waypoint").at("description");

    if (t.contains("type")) {
      if (t.at("type") == "TAKEOFF") {
        auto &&parsed = std::make_shared<Takeoff>(GeoPoint(lat, lon, alt),
                                                  radius, name, desc);
        this->m_takeoff = parsed;
        this->m_all_tp.push_back(parsed);
      } else if (t.at("type") == "SSS") {
        json sss_windows = j.at("sss").at("timeGates");
        if (sss_windows.size() > 1) {
          throw std::runtime_error(
              "Multiple SSS windows are not supported"); // TODO test
        }
        XCTaskTime open(std::string(sss_windows.front()));
        auto &&parsed = std::make_shared<SSS>(GeoPoint(lat, lon, alt), radius,
                                              name, desc, open);
        this->m_sss = parsed;
        this->m_all_tp.push_back(parsed);
      } else if (t.at("type") == "ESS") {
        auto parsed =
            std::make_shared<ESS>(GeoPoint(lat, lon, alt), radius, name, desc);
        this->m_ess = parsed;
        this->m_all_tp.push_back(parsed);
      }
    }
    // GOAL is the last turpoint of the array
    else if (&t == &j.at("turnpoints").back()) {
      bool line = j.at("goal").at("type") == "LINE";
      XCTaskTime deadline(std::string(j.at("goal").at("deadline")));
      auto &&parsed = std::make_shared<Goal>(GeoPoint(lat, lon, alt), radius,
                                             name, desc, deadline, line);
      this->m_goal = parsed;
      this->m_all_tp.push_back(parsed);
    } else {
      auto &&parsed = std::make_shared<Turnpoint>(GeoPoint(lat, lon, alt),
                                                  radius, name, desc);
      this->m_all_tp.push_back(parsed);
      this->m_tp.push_back(parsed);
    }
    logging::debug({"[ TASK ]", this->m_all_tp.back()->to_string()});
  }

  this->flatten();
}
