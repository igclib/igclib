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
  if (t < this->m_track_on) {
    return this->m_status.at(this->m_track_on);
  } else if (t > this->m_track_off) {
    return this->m_status.at(this->m_track_off);
  } else {
    return this->m_status.at(t);
  }
}

json RaceStatus::to_json() const {
  json j = this->m_position->to_json();
  j["goal_distance"] = this->m_goal_distance;
  switch (this->m_status) {
  case FlightStatus::ON_TAKEOFF:
    j["status"] = "ON_TAKEOFF";
    break;
  case FlightStatus::IN_FLIGHT:
    j["status"] = "IN_FLIGHT";
    break;
  case FlightStatus::LANDED:
    j["status"] = "LANDED";
    break;
  case FlightStatus::UNKNOWN:
    j["status"] = "UNKNOWN";
    break;
  case FlightStatus::IN_GOAL:
    j["status"] = "IN_GOAL";
    break;
  default:
    throw std::out_of_range("unknown flight status");
  }
  return j;
}

const Time &RaceFlight::takeoff_time() const { return this->m_takeoff; }
const Time &RaceFlight::landing_time() const { return this->m_landing; }

RaceStatus::RaceStatus(std::shared_ptr<GeoPoint> pos, double dist_to_goal,
                       const FlightStatus &status)
    : m_position(pos), m_status(status), m_goal_distance(dist_to_goal) {}

void RaceFlight::validate(const Task &task) {
  auto remaining_centers = task.centers();
  auto remaining_radii = task.radii();
  auto it = task.begin();
  std::shared_ptr<Turnpoint> current_tp = *it;
  std::shared_ptr<GeoPoint> pos;
  RaceStatus status;
  Route route;
  vec init_vec = Route::gen_init(remaining_centers.size());
  double dist_to_goal = task.length();
  const double TOL = 10;

  for (Time t = this->m_track_on; t <= this->m_track_off; ++t) {
    // try to get the pilot position at t
    try {
      pos = this->m_points.at(t);
    } catch (const std::out_of_range &err) {
      status = RaceStatus(pos, dist_to_goal, FlightStatus::UNKNOWN);
    }

    // before takeoff and after landing, goal distances are not recomputed
    if (t < this->m_takeoff) {
      status = RaceStatus(pos, dist_to_goal, FlightStatus::ON_TAKEOFF);
    } else if (t > this->m_landing) {
      status = RaceStatus(pos, dist_to_goal, FlightStatus::LANDED);
    } else if (!remaining_centers.empty()) {
      route = Route(*pos, remaining_centers, remaining_radii, init_vec);
      init_vec = route.opt_vec();

      status =
          RaceStatus(pos, route.optimal_distance(), FlightStatus::IN_FLIGHT);
      // can only validate takoff before start
      if (t < task.start()) {
        if (current_tp == task.takeoff()) {
          this->m_tag_times.push_back(t);
          current_tp = *(++it);
          remaining_centers.pop_front();
          remaining_radii.pop_front();
        }
      }
      // cannot validate anything after task close
      else if (t > task.close()) {
      }
      // validate next tp if border is "close enough"
      // TODO make validation in/out or out/in
      else {
        if (route.legs().front() <= TOL) {
          this->m_tag_times.push_back(t);
          current_tp = *(++it);
          remaining_centers.pop_front();
          remaining_radii.pop_front();
        }
      }
    }
    // if no remaining turnpoints, pilot is in goal
    else {
      status = RaceStatus(pos, 0, FlightStatus::IN_GOAL);
    }

    this->m_status.emplace(t, status);
  }
}