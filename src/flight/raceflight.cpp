#include <igclib/flight.hpp>
#include <igclib/logging.hpp>
#include <igclib/task.hpp>
#include <string>

RaceFlight::RaceFlight(const std::string &igc_file, const Task &task)
    : Flight(igc_file) {
  _track_on = m_points.timepoints().cbegin()->first;
  m_takeoff = _track_on; // TODO identify takeoff with speed
  m_track_off = m_points.timepoints().crbegin()->first;
  m_landing = m_track_off; // TODO identify landing with speed

  if (task.task_style() == TaskStyle::RACE_TO_GOAL) {
    this->validate_race(task);
  } else if (task.task_style() == TaskStyle::ELAPSED_TIME) {
    this->validate_elapsed(task);
  }

  m_in_goal = m_tag_times.size() == task.n_turnpoints();
}

RaceStatus RaceFlight::at(const Time &t) const {
  if (t < _track_on) {
    return _status.at(_track_on);
  } else if (t > m_track_off) {
    return _status.at(m_track_off);
  } else {
    return _status.at(t);
  }
}

/**
 * @brief Computes the best elasped time attempt on the given task
 *
 * @note This is only correct on a very specific use case, do not assume it
 * handles every possible task
 *
 */
void RaceFlight::validate_elapsed(const Task &task) {
  std::shared_ptr<GeoPoint> prev_pos;
  std::shared_ptr<GeoPoint> pos = m_points.cbegin()->second;
  std::shared_ptr<SSS> sss = task.sss();
  std::shared_ptr<ESS> ess = task.ess();

  std::vector<std::vector<Time>> attempts;

  bool has_to_enter = sss->is_enter();
  bool is_inside = sss->contains(*pos);
  bool prev_is_inside;
  bool crossed_tp;

  // identify all start crossings
  for (auto it = m_points.cbegin(); it != m_points.cend(); ++it) {
    prev_pos = pos;
    prev_is_inside = is_inside;
    pos = it->second;
    is_inside = sss->contains(*pos);

    crossed_tp = (prev_is_inside != is_inside);
    if (crossed_tp) {
      if ((has_to_enter && is_inside) || (!has_to_enter && !is_inside)) {
        attempts.push_back({it->first});
      }
    }
  }

  // get first turnpoint after SSS
  auto first_it = task.begin();
  while (*first_it != sss) {
    first_it++;
  };
  first_it++;

  // for each start, compute time to ess
  for (auto &attempt : attempts) {
    auto current_tp = first_it;
    Time t = attempt.front();
    pos = m_points.at(t);
    is_inside = current_tp->get()->contains(*pos);
    has_to_enter = !is_inside;
    while (t < m_landing) {
      ++t;
      prev_pos = pos;
      prev_is_inside = is_inside;
      pos = m_points.at(t);
      is_inside = current_tp->get()->contains(*pos);
      crossed_tp = (prev_is_inside != is_inside);
      if (crossed_tp) {
        if ((has_to_enter && is_inside) || (!has_to_enter && !is_inside)) {
          attempt.push_back(t);
          if (*current_tp == ess) {
            break;
          } else {
            current_tp++;
            has_to_enter = !current_tp->get()->contains(*pos);
          }
        }
      }
    }
  }

  std::vector<Time> conclusive_attempts;
  auto best_it = attempts.end();
  Time best_time(23, 59, 59);
  for (auto it = attempts.begin(); it != attempts.end(); ++it) {
    if (it->size() == task.n_turnpoints()) {
      Time attempt_time = it->back() - it->front();
      if (attempt_time < best_time) {
        best_it = it;
        best_time = attempt_time;
      }
    }
  }

  if (best_it != attempts.end()) {
    m_race_time = best_time;
    Time start_time = *best_it->begin();
    for (const Time &t : *best_it) {
      m_tag_times.push_back(t - start_time);
    }
  }
}

void RaceFlight::validate_race(const Task &task) {
  auto remaining_centers = task.centers();
  auto remaining_radii = task.radii();
  auto it = task.begin();
  std::shared_ptr<Turnpoint> current_tp = *it;
  std::shared_ptr<GeoPoint> pos;
  FlightStatus status;
  Route route;
  double dist_to_goal = task.length();
  bool has_to_enter = task.sss()->is_enter();

  for (Time t = _track_on; t <= m_track_off; ++t) {
    // try to get the pilot position at current time
    try {
      pos = m_points.at(t);
    } catch (const std::out_of_range &err) {
      status = FlightStatus::UNKNOWN;
    }

    // before takeoff and after landing, goal distances are not recomputed
    if (t < m_takeoff) {
      status = FlightStatus::ON_TAKEOFF;
    } else if (t > m_landing) {
      status = FlightStatus::LANDED;
    } else {
      status = FlightStatus::IN_FLIGHT;
      // if there are still turnpoints to validate
      if (!remaining_centers.empty()) {
        // first make sure the task is not closed
        if (task.has_close_time() && t > task.close()) {
          // TODO maybe some status ?
        } else {

          // route = Route(*pos, remaining_centers, remaining_radii,
          // init_vec); init_vec = route.opt_theta();

          // takeoff validation
          if (current_tp == task.takeoff() && current_tp->contains(*pos)) {
            m_tag_times.push_back(t);
            current_tp = *(++it);
            remaining_centers.pop_front();
            remaining_radii.pop_front();
          }

          // start validation
          else if (current_tp == task.sss() && t > task.start()) {
            bool is_inside = current_tp->contains(*pos);
            if ((has_to_enter && is_inside) || (!has_to_enter && !is_inside)) {
              m_tag_times.push_back(t);
              current_tp = *(++it);
              remaining_centers.pop_front();
              remaining_radii.pop_front();
              has_to_enter = !current_tp->contains(*pos);
            }
          }
          // validate next tp if border is crossed
          else if (current_tp == task.ess() && current_tp->contains(*pos)) {
            m_race_time = t - task.start();
            m_tag_times.push_back(t);
            current_tp = *(++it);
            remaining_centers.pop_front();
            remaining_radii.pop_front();
          } else if (current_tp == task.goal() && current_tp->contains(*pos)) {
            m_tag_times.push_back(t);
            current_tp = *(++it);
            remaining_centers.pop_front();
            remaining_radii.pop_front();
          } else {
            bool is_inside = current_tp->contains(*pos);
            if ((has_to_enter && is_inside) || (!has_to_enter && !is_inside)) {
              m_tag_times.push_back(t);
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

json RaceFlight::to_json() const {
  json j;
  j["in_goal"] = m_in_goal;
  if (m_in_goal) {
    j["race_time_sec"] = m_race_time.to_seconds();
    for (const Time &t : m_tag_times) {
      j["tag_times"].push_back(t.to_seconds());
    }
  }
  return j;
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
