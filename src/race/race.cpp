#include <boost/filesystem.hpp>
#include <igclib/json.hpp>
#include <igclib/logging.hpp>
#include <igclib/race.hpp>
#include <igclib/task.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>

Race::Race(const std::string &flight_dir, const std::string &task_file)
    : _first_takeoff(23, 59, 59), _last_landing(0, 0, 0), _task(task_file) {

  // parse flights
  boost::filesystem::path p(flight_dir);
  if (!boost::filesystem::is_directory(p)) {
    const std::string err = flight_dir + " is not a directory";
    throw std::runtime_error(err);
  }

  std::string filename;
  std::vector<std::thread> jobs;
  std::mutex mutex;
  for (const auto &file : boost::filesystem::directory_iterator(p)) {
    if (boost::filesystem::is_regular_file(file)) {
      filename = file.path().string();
      jobs.emplace_back(std::thread([&, filename] {
        try {
          auto ptr = std::make_shared<RaceFlight>(filename, _task);
          std::lock_guard<std::mutex> lock(mutex);
          _first_takeoff = std::min(_first_takeoff, ptr->takeoff_time());
          _last_landing = std::max(_last_landing, ptr->landing_time());
          _flights.push_back(ptr);
          logging::debug(
              {"[ RACE ]", ptr->pilot(), ptr->race_time().to_string()});
        } catch (const std::runtime_error &err) {
          logging::debug({"[ RACE ] invalid flight :", err.what()});
        }
      }));
    }
  }
  for (auto &j : jobs) {
    j.join();
  }

  // build snapshots
  _snapshots.reserve(_flights.size());
  for (Time t = _first_takeoff; t <= _last_landing; ++t) {
    auto snap = std::make_shared<Snapshot>(t);
    for (auto flight : _flights) {
      snap->add(flight->id(), flight->at(t));
    }
    _snapshots.push_back(snap);
  }

  logging::debug(
      {"[ RACE ]", std::to_string(_flights.size()), "flights initialized"});
  logging::debug({"[ RACE ] first takeoff", _first_takeoff.to_string()});
  logging::debug({"[ RACE ] last landing", _last_landing.to_string()});
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
  j["task"] = _task.to_json();

  for (const auto &snap : _snapshots) {
    j["snapshots"][snap->time().to_string()] = snap->to_json();
  }

  return j;
}

void Snapshot::add(const std::string &id, const RaceStatus &status) {
  _status.emplace_back(id, status);
}

json Snapshot::to_json() const {
  json j;
  for (const auto &p : _status) {
    j[p.first] = p.second.to_json();
  }
  return j;
}