#include <igclib/flight.hpp>
#include <igclib/logging.hpp>
#include <igclib/task.hpp>
#include <string>

RaceFlight::RaceFlight(const std::string &igc_file, const Task &task)
    : Flight(igc_file) {
  _track_on = _points.timepoints().cbegin()->first;
  _takeoff = _track_on; // TODO identify takeoff with speed
  _track_off = _points.timepoints().crbegin()->first;
  _landing = _track_off; // TODO identify landing with speed

  this->validate(task);
}

RaceStatus RaceFlight::at(const Time &t) const {
  if (t < _track_on) {
    return _status.at(_track_on);
  } else if (t > _track_off) {
    return _status.at(_track_off);
  } else {
    return _status.at(t);
  }
}

json RaceStatus::to_json() const {
  json j = _position->to_json();
  j["goal_distance"] = _goal_distance;
  switch (_status) {
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
    : _position(pos), _status(status), _goal_distance(dist_to_goal) {}

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

  for (Time t = _track_on; t <= _track_off; ++t) {
    // try to get the pilot position at current time
    try {
      pos = _points.at(t);
    } catch (const std::out_of_range &err) {
      status = FlightStatus::UNKNOWN;
    }

    // before takeoff and after landing, goal distances are not recomputed
    if (t < _takeoff) {
      status = FlightStatus::ON_TAKEOFF;
    } else if (t > _landing) {
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
            _tag_times.push_back(t);
            current_tp = *(++it);
            remaining_centers.pop_front();
            remaining_radii.pop_front();
          }

          // start validation
          else if (current_tp == task.sss() && t > task.start()) {
            bool is_inside = current_tp->contains(*pos);
            if ((has_to_enter && is_inside) || (!has_to_enter && !is_inside)) {
              _tag_times.push_back(t);
              current_tp = *(++it);
              remaining_centers.pop_front();
              remaining_radii.pop_front();
              has_to_enter = !current_tp->contains(*pos);
            }
          }
          // validate next tp if border is crossed
          else if (current_tp == task.ess() && current_tp->contains(*pos)) {
            _race_time = t - task.start();
            _tag_times.push_back(t);
            current_tp = *(++it);
            remaining_centers.pop_front();
            remaining_radii.pop_front();
          } else if (current_tp == task.goal() && current_tp->contains(*pos)) {
            _tag_times.push_back(t);
            current_tp = *(++it);
            remaining_centers.pop_front();
            remaining_radii.pop_front();
          } else {
            bool is_inside = current_tp->contains(*pos);
            if ((has_to_enter && is_inside) || (!has_to_enter && !is_inside)) {
              _tag_times.push_back(t);
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
    _status.emplace(t, RaceStatus(pos, dist_to_goal, status));
  }
}
