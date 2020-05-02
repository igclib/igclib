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

RaceStatus::RaceStatus(std::shared_ptr<GeoPoint> pos, double dist_to_goal,
                       const FlightStatus &status)
    : m_position(pos), m_status(status), m_goal_distance(dist_to_goal) {}

void RaceFlight::validate(const Task &task) {
  auto remaining_centers = task.centers();
  auto remaining_radii = task.radii();
  auto it = task.begin();
  std::shared_ptr<Turnpoint> current_tp = *it;
  std::shared_ptr<GeoPoint> pos;
  FlightStatus status;
  Route route;
  double dist_to_goal = task.length();
  bool has_to_enter = task.sss()->is_enter();

  for (Time t = this->m_track_on; t <= this->m_track_off; ++t) {
    // try to get the pilot position at current time
    try {
      pos = this->m_points.at(t);
    } catch (const std::out_of_range &err) {
      status = FlightStatus::UNKNOWN;
    }

    // before takeoff and after landing, goal distances are not recomputed
    if (t < this->m_takeoff) {
      status = FlightStatus::ON_TAKEOFF;
    } else if (t > this->m_landing) {
      status = FlightStatus::LANDED;
    } else {
      status = FlightStatus::IN_FLIGHT;
      // if there are still turnpoints to validate
      if (!remaining_centers.empty()) {
        // first make sure the task is not closed
        if (t > task.close()) {
          // TODO maybe some status ?
        } else {

          // route = Route(*pos, remaining_centers, remaining_radii, init_vec);
          // init_vec = route.opt_theta();

          // takeoff validation
          if (current_tp == task.takeoff() && current_tp->contains(*pos)) {
            this->m_tag_times.push_back(t);
            current_tp = *(++it);
            remaining_centers.pop_front();
            remaining_radii.pop_front();
          }

          // start validation
          else if (current_tp == task.sss() && t > task.start()) {
            bool is_inside = current_tp->contains(*pos);
            if ((has_to_enter && is_inside) || (!has_to_enter && !is_inside)) {
              this->m_tag_times.push_back(t);
              current_tp = *(++it);
              remaining_centers.pop_front();
              remaining_radii.pop_front();
              has_to_enter = !current_tp->contains(*pos);
            }
          }
          // validate next tp if border is crossed
          else if (current_tp == task.ess() && current_tp->contains(*pos)) {
            this->m_race_time = t - task.start();
            this->m_tag_times.push_back(t);
            current_tp = *(++it);
            remaining_centers.pop_front();
            remaining_radii.pop_front();
          } else if (current_tp == task.goal() && current_tp->contains(*pos)) {
            this->m_tag_times.push_back(t);
            current_tp = *(++it);
            remaining_centers.pop_front();
            remaining_radii.pop_front();
          } else {
            bool is_inside = current_tp->contains(*pos);
            if ((has_to_enter && is_inside) || (!has_to_enter && !is_inside)) {
              this->m_tag_times.push_back(t);
              current_tp = *(++it);
              remaining_centers.pop_front();
              remaining_radii.pop_front();
              has_to_enter = !current_tp->contains(*pos);
            }
          }
        }
      }
      // if no remaining turnpoints, pilot is in goal
      else {
        status = FlightStatus::IN_GOAL;
        dist_to_goal = 0;
      }
    }
    this->m_status.emplace(t, RaceStatus(pos, dist_to_goal, status));
  }
}
