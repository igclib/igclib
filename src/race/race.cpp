#include <boost/filesystem.hpp>
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
        RaceFlight flight(filename);
        logging::debug({"[ RACE ] ", flight.pilot()});
      } catch (const std::runtime_error &err) {
        logging::debug({"[ RACE ] invalid flight :", err.what()});
      }
    }
  }
}

void Race::save(const std::string &out) const {}