#include <cpr/cpr.h>
#include <fstream>
#include <igclib/airspace.hpp>
#include <igclib/flight.hpp>
#include <igclib/geopoint.hpp>
#include <igclib/pointcollection.hpp>
#include <igclib/time.hpp>
#include <igclib/util.hpp>
#include <iostream>
#include <nlohmann/json.hpp>
#include <queue>
#include <stdexcept>
#include <string>

using json = nlohmann::json;

Flight::Flight(const std::string &igc_file) {
  // read and parse igc file
  std::ifstream f;
  f.open(igc_file);
  if (!f.is_open()) {
    const std::string error = "Could not open file '" + igc_file + "'";
    throw std::runtime_error(error);
  }

  std::string line;
  while (std::getline(f, line)) {
    switch (line[0]) {
    // header records
    case 'H':
      process_H_record(line);
      break;
    // fix records
    case 'B':
      process_B_record(line);
    default:
      break;
    }
  }

  f.close();
}

void Flight::process_H_record(const std::string &record) {
  const std::string record_code = record.substr(2, 3);
  if (record_code == "PLT") {
    size_t delim = record.find(':') + 1;
    this->pilot_name = record.substr(delim);
    util::trim(this->pilot_name);
  } else if (record_code == "TZO") {
    size_t delim = record.find(':') + 1;
    this->time_zone_offset = std::stoi(record.substr(delim));
  }
}

void Flight::process_B_record(const std::string &record) {
  IGCTime t(record, this->time_zone_offset);
  IGCPoint p(record);
  this->points.insert(t, p);
}

// Returns the JSON serialization of a Flight
json Flight::serialize() const {
  json j = {{"pilot", this->pilot_name}};

  j["infractions"] = json::array();
  for (const auto &infraction : this->infractions) {
    j["infractions"][infraction.first];
    for (const GeoPoint &p : infraction.second) {
      j["infractions"][infraction.first].push_back(p.serialize());
    }
  }

  if (this->score) {
    j["xc_info"] = this->score; //.serialize();
  } else {
    j["xc_info"] = "Not computed";
  }

  return j;
}

void Flight::save(const std::string &out) const {
  json j = serialize();

  if (out == "-" || out.empty()) {
    std::cout << j.dump(4) << std::endl;
  } else {
    std::ofstream f;
    f.open(out);
    if (!f.is_open()) {
      const std::string error = "Could not open file '" + out + "'";
      throw std::runtime_error(error);
    }
    f << j.dump(4);
    f.close();
  }
}

void Flight::validate(const Airspace &airspace) {
  bool is_agl_validable = false;

  // retrieve the ground altitude for each point of the flight
  if (airspace.need_agl_checking) {
    if (getenv("ELEVATION_API_KEY")) {
      std::string key = getenv("ELEVATION_API_KEY");
      std::string api = "https://geolocalisation.ffvl.fr/elevation?key=" + key;
      json data = this->points.latlon();
      cpr::Session session;
      cpr::Body body(data.dump());
      session.SetVerifySsl(false);
      session.SetUrl(api);
      session.SetBody(body);
      cpr::Response r = session.Post();
      if (r.status_code == 200) {
        data = json::parse(r.text);
        std::vector<double> altitudes = data.get<std::vector<double>>();
        if (this->points.set_agl(altitudes)) {
          is_agl_validable = true;
        }
      } else if (r.status_code == 400) {
        std::cerr << "Missing or invalid API key for the elevation service."
                  << std::endl;
      } else {
        std::cerr << "Elevation service could not be reached." << std::endl;
      }
    } else {
      std::cerr << "This airspace file contains zones which need to be checked "
                   "against ground altitude of the flight, but the environment "
                   "variable 'ELEVATION_API_KEY' is not set. These zones will "
                   "not be considered."
                << std::endl;
    }
  }

  // then validate intersections
  airspace.infractions(this->points, this->infractions, is_agl_validable);
}

void Flight::compute_score() {
  // Inspired by by Ondrej Palkovsky
  // http://www.penguin.cz/~ondrap/algorithm.pdf

  this->score = this->heuristic_score();
  // this->optimize(FreeObjectiveFunction(), FreeBoundingFunction());
  this->optimize(TriangleObjectiveFunction(), TriangleBoundingFunction());
}

double Flight::heuristic_score() {
  const int n_approx = 100; // number of points used for tracklog approximation
  const int step_ratio = this->points.size() / n_approx;

  std::vector<double> previous_distances(n_approx);
  std::vector<double> next_distances(n_approx);

  // n_steps must be number of points - 1
  int n_steps = 4;
  for (int step = 0; step < n_steps; ++step) {
    for (std::size_t i = 0; i < n_approx; ++i) {
      double max_distance = 0;
      for (std::size_t j = 0; j < i; ++j) {
        double candidate =
            previous_distances[j] +
            this->points[i * step_ratio].distance(this->points[j * step_ratio]);
        if (candidate > max_distance) {
          max_distance = candidate;
        }
      }
      next_distances[i] = max_distance;
    }
    previous_distances = next_distances;
  }

  return *std::max_element(next_distances.begin(), next_distances.end());
}

void Flight::optimize(const ObjectiveFunction &score,
                      const BoundingFunction &bound) {
  std::vector<std::size_t> initial_points = {
      5}; // TODO make this a parameter, or is there 5 points in any case?
  auto initial_pair =
      std::make_pair((std::size_t)0, (std::size_t)this->points.size());
  std::vector<std::pair<std::size_t, std::size_t>> initial_box = {initial_pair};

  std::priority_queue<CandidateTree> candidates;
  CandidateTree initial_candidate(initial_points, initial_box);
  bound(initial_candidate, *this);
  candidates.push(initial_candidate);

  double current_score = 0;
  while (!candidates.empty()) {
    CandidateTree node = candidates.top();
    candidates.pop();

    if (node.is_single_candidate()) {
      current_score = score(node, *this);
      if (current_score > this->score) {
        this->score = current_score;
      }
    } else {
      for (auto &&child : node.branch(*this)) {
        if (bound(child, *this) >= this->score) {
          candidates.push(child);
        }
      }
    }
  }
}

bool Flight::is_closed(std::pair<std::size_t, std::size_t> start_box,
                       std::pair<std::size_t, std::size_t> end_box,
                       double max_distance) const {
  // first, check for closing on bounding box ?
  std::size_t n_approx_start = (start_box.second - start_box.first) / 5;
  std::size_t n_approx_end = (end_box.second - end_box.first) / 5;
  for (std::size_t i = start_box.first; i <= start_box.second;
       i += n_approx_start) {
    for (std::size_t j = end_box.first; j < end_box.second; j += n_approx_end) {
      if (this->points.at(i).distance(this->points.at(j)) < max_distance) {
        return true;
      }
    }
  }
  return false;
}

bool CandidateTree::is_single_candidate() {
  for (std::size_t i = 0; i < this->v_boxes.size(); i++) {
    auto box_size = this->v_boxes.at(i).second - this->v_boxes.at(i).first;
    if (box_size > 10) { // TODO figure out if 1 is correct
      return false;
    }
  }
  return true;
}

std::vector<CandidateTree> CandidateTree::branch(const Flight &flight) {
  std::vector<CandidateTree> branches = {};

  std::size_t picked_index = 0;
  std::size_t largest_box = 0;
  for (std::size_t i = 0; i < this->v_boxes.size(); i++) {
    // eager branching of box sharing points
    if (this->v_points.at(i) > 1) {
      picked_index = i;
      break;
    }
    // branching on box with largest diagonal
    auto box_size = flight.max_diagonal(this->v_boxes.at(i));
    if (box_size > largest_box) {
      picked_index = i;
      largest_box = box_size;
    }
  }

  auto n = this->v_points[picked_index];
  auto picked_box = this->v_boxes[picked_index];
  auto half_point = (picked_box.first + picked_box.second) / 2;

  for (std::size_t i = 0; i <= n; i++) {
    // all elements must be deep copied to not modify original objects
    auto first_half = std::make_pair(picked_box.first, half_point);
    auto second_half = std::make_pair(half_point, picked_box.second);
    CandidateTree new_candidate(*this);
    new_candidate.set_score(-1); // FIXME

    auto box_it = new_candidate.v_boxes.begin() + picked_index + 1;
    auto points_it = new_candidate.v_points.begin() + picked_index + 1;

    if (n - i > 0) {
      new_candidate.v_points.insert(points_it, n - i);
      new_candidate.v_boxes.insert(box_it, second_half);
    }
    if (i > 0) {
      new_candidate.v_points.at(picked_index) = i;
      new_candidate.v_boxes.at(picked_index) = first_half;
    } else {
      auto points_to_erase = new_candidate.v_points.begin() + picked_index;
      auto box_to_erase = new_candidate.v_boxes.begin() + picked_index;
      new_candidate.v_points.erase(points_to_erase);
      new_candidate.v_boxes.erase(box_to_erase);
    }

    branches.push_back(new_candidate);
  }

  return branches;
}

/* FreeObjectiveFunction */

double FreeObjectiveFunction::operator()(const CandidateTree &node,
                                         const Flight &flight) const {
  (void)flight;
  return node.score();
}

double TriangleObjectiveFunction::operator()(const CandidateTree &node,
                                             const Flight &flight) const {
  (void)flight;
  return node.score();
}

/* FreeBoundingFunction */

double FreeBoundingFunction::operator()(CandidateTree &node,
                                        const Flight &flight) const {
  if (node.score() != -1) {
    return node.score();
  }

  std::vector<std::vector<GeoPoint>> bboxes;
  for (std::size_t i = 0; i < node.v_points.size(); i++) {
    auto box = flight.bbox(node.v_boxes.at(i));
    bboxes.insert(bboxes.end(), node.v_points.at(i), box);
  }

  double best_score = 0;
  double current_score = 0;
  GeoPoint previous_point;
  for (auto combination : util::product<GeoPoint>(bboxes)) {
    current_score = 0;
    previous_point = combination.at(0);
    for (std::size_t i = 1; i < combination.size(); i++) {
      current_score += previous_point.distance(combination.at(i));
      previous_point = combination.at(i);
    }
    if (current_score > best_score) {
      best_score = current_score;
    }
  }
  node.set_score(best_score);

  return best_score;
}

double TriangleBoundingFunction::operator()(CandidateTree &node,
                                            const Flight &flight) const {
  if (node.score() != -1) {
    return node.score();
  }

  std::vector<std::vector<GeoPoint>> bboxes;
  for (std::size_t i = 0; i < node.v_points.size(); i++) {
    auto box = flight.bbox(node.v_boxes.at(i));
    bboxes.insert(bboxes.end(), node.v_points.at(i), box);
  }

  double best_score = 0;
  double current_score = 0;
  double accepted_closing;
  for (auto &&combination : util::product<GeoPoint>(bboxes)) {
    current_score = combination.at(1).distance(combination.at(2));
    current_score += combination.at(2).distance(combination.at(3));
    current_score += combination.at(3).distance(combination.at(1));
    if (current_score > best_score) {
      accepted_closing = std::max(3000.0, 0.05 * current_score);
      if (flight.is_closed(node.v_boxes.front(), node.v_boxes.back(),
                           accepted_closing)) {
        best_score = current_score;
      }
    }
  }

  best_score *= 1.2; // triangle coefficient
  node.set_score(best_score);
  return best_score;
}