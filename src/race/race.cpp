#include <boost/filesystem.hpp>
#include <igclib/json.hpp>
#include <igclib/logging.hpp>
#include <igclib/race.hpp>
#include <igclib/task.hpp>
#include <memory>
#include <stdexcept>
#include <string>

Race::Race(const std::string &flight_dir, const std::string &task_file) {
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
        auto ptr = std::make_shared<RaceFlight>(filename);
        this->m_flights.push_back(ptr);
        logging::debug({"[ RACE ]", ptr->pilot()});
      } catch (const std::runtime_error &err) {
        logging::debug({"[ RACE ] invalid flight :", err.what()});
      }
    }
  }

  // score flights on task
  for (auto f : this->m_flights) {
    f->score(*this->m_task);
  }
}

void Race::save(const std::string &out) const {
  json j = this->to_json();

  if (out == "-" || out.empty()) {
    std::cout << j.dump(4, ' ', false, json::error_handler_t::ignore)
              << std::endl;
  } else {
    std::ofstream f;
    f.open(out);
    if (!f.is_open()) {
      const std::string error = "could not open file '" + out + "'";
      throw std::runtime_error(error);
    }
    f << j.dump(4, ' ', false, json::error_handler_t::ignore);
  }
}

json Race::to_json() const {
  json j;
  j["task"] = this->m_task->to_json();
  j["snapshots"];

  for (Time t = this->start(); t <= this->close(); ++t) {
    for (auto flight : this->m_flights)
      if (t >= flight->first_instant() && t <= flight->last_instant()) {
        try {
          j["snapshots"][t.to_string()][flight->id()] =
              flight->at(t)->to_json();
        } catch (const std::out_of_range &err) {
          logging::debug(
              {"[ RACE ] no point for", flight->id(), "at", t.to_string()});
        }
      }
  }
  return j;
}

const Time &Race::start() const { return this->m_task->start(); }
const Time &Race::close() const { return this->m_task->close(); }