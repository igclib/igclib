#include <igclib/flight.hpp>
#include <igclib/json.hpp> //?
#include <igclib/task.hpp>
#include <string>

RaceFlight::RaceFlight(const std::string &igc_file) : Flight(igc_file) {
  this->m_first_instant = this->m_points.timepoints().cbegin()->first;
  this->m_last_instant = this->m_points.timepoints().crbegin()->first;
}

const Time &RaceFlight::first_instant() const { return this->m_first_instant; }
const Time &RaceFlight::last_instant() const { return this->m_last_instant; }

void RaceFlight::score(const Task &task) { Task remaining(task); }