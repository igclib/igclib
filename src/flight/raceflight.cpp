#include <igclib/flight.hpp>
#include <igclib/logging.hpp>
#include <igclib/task.hpp>
#include <string>

RaceFlight::RaceFlight(const std::string &igc_file, const Task &task)
    : Flight(igc_file) {
  this->m_track_on = this->m_points.timepoints().cbegin()->first;
  this->m_takeoff = this->m_track_on; // TODO identify takeoff with speed
  this->m_track_off = this->m_points.timepoints().crbegin()->first;
  this->m_landing = this->m_track_off; // TODO identify landing with speed

  this->validate(task);
}

RaceStatus RaceFlight::at(const Time &t) const {
  try {
    if (t < this->m_takeoff) {
      return this->m_status.at(this->m_takeoff);
    } else if (t > this->m_landing) {
      return this->m_status.at(this->m_landing);
    } else {
      return this->m_status.at(t);
    }
  } catch (const std::out_of_range &err) {
    logging::debug(
        {"[ FLIGHT ] no point for", this->id(), "at", t.to_string()});
    return RaceStatus();
  }
}

json RaceStatus::to_json() const {
  json j = this->m_position->to_json();
  j["goal_distance"] = this->goal_distance;
  return j;
}

const Time &RaceFlight::takeoff_time() const { return this->m_takeoff; }
const Time &RaceFlight::landing_time() const { return this->m_landing; }

void RaceFlight::validate(const Task &task) {
  std::size_t tp_idx = 1;
  std::shared_ptr<Turnpoint> next_tp = task.at(tp_idx);
  Time time(this->m_track_on);
  std::shared_ptr<GeoPoint> pos = this->m_points.at(time);
  RaceStatus status;
  Route route;

  while (++time < this->m_landing) {
    try {
      pos = this->m_points.at(time);
    } catch (const std::out_of_range &err) {
      logging::debug({"[ TASK VALIDATION ] no point for", this->pilot(), "at",
                      time.to_string()});
    }
    route = Route();
    status = RaceStatus();
    if (time < task.start()) {
    }
    this->m_status.emplace(time, status);
  }
}