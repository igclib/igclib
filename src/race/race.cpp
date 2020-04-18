#include <boost/filesystem.hpp>
#include <igclib/json.hpp>
#include <igclib/logging.hpp>
#include <igclib/race.hpp>
#include <igclib/task.hpp>
#include <memory>
#include <stdexcept>
#include <string>

Race::Race(const std::string &flight_dir, const std::string &task_file)
    : m_first_takeoff(23, 59, 59), m_last_landing(0, 0, 0) {
  // create task
  this->m_task = std::make_unique<Task>(task_file);

  // create flights
  boost::filesystem::path p(flight_dir);
  if (!boost::filesystem::is_directory(p)) {
    const std::string err = flight_dir + " is not a directory";
    throw std::runtime_error(err);
  }

  std::string filename;
  for (const auto &file : boost::filesystem::directory_iterator(p)) {
    if (boost::filesystem::is_regular_file(file)) {
      filename = file.path().string();
      try {
        auto ptr = std::make_shared<RaceFlight>(filename, *this->m_task);
        this->m_first_takeoff =
            std::min(this->m_first_takeoff, ptr->takeoff_time());
        this->m_last_landing =
            std::max(this->m_last_landing, ptr->landing_time());
        this->m_flights.push_back(ptr);
        logging::debug({"[ RACE ]", ptr->pilot()});
      } catch (const std::runtime_error &err) {
        logging::debug({"[ RACE ] invalid flight :", err.what()});
      }
    }
  }

  this->m_snapshots.reserve(this->m_flights.size());
  for (Time t = this->m_first_takeoff; t <= this->m_last_landing; ++t) {
    auto snap = std::make_shared<Snapshot>(t);
    for (auto flight : this->m_flights) {
      snap->add(flight->id(), flight->at(t));
    }
    this->m_snapshots.push_back(snap);
  }

  logging::debug({"[ RACE ] initialized"});
}

void Race::save(const std::string &out) const {
  json j = this->to_json();

  if (out == "-" || out.empty()) {
    std::cout << j.dump(4, ' ', false, json::error_handler_t::replace)
              << std::endl;
  } else {
    std::ofstream f;
    f.open(out);
    if (!f.is_open()) {
      const std::string error = "could not open file '" + out + "'";
      throw std::runtime_error(error);
    }
    f << j.dump(4, ' ', false, json::error_handler_t::replace);
  }
}

json Race::to_json() const {
  json j;
  j["task"] = this->m_task->to_json();

  for (const auto &snap : this->m_snapshots) {
    j["snapshots"][snap->time().to_string()] = snap->to_json();
  }

  return j;
}

Snapshot::Snapshot(const Time &t) : m_time(t) {}

void Snapshot::add(const std::string &id, const RaceStatus &status) {
  this->m_status.push_back(std::make_pair(id, status));
}

json Snapshot::to_json() const {
  json j;
  for (const auto &p : this->m_status) {
    j[p.first] = p.second.to_json();
  }
  return j;
}