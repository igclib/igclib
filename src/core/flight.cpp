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

Flight::Flight(const std::string &flight_file) {
  // read and parse igc file
  std::ifstream f;
  f.open(flight_file);
  if (!f.is_open()) {
    const std::string error = "Could not open file '" + flight_file + "'";
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
  // Algorithm designed by Ondrej Palkovsky
  // http://www.penguin.cz/~ondrap/algorithm.pdf

  this->score = this->heuristic_score();
  this->optimize(FreeObjectiveFunction(), FreeBoundingFunction());

  std::cout << this->score << std::endl;
}

double Flight::heuristic_score() {
  const int n_approx = 100; // number of points used for tracklog approximation
  const int step_ratio = this->points.size() / n_approx;

  std::vector<int> previous_distances(n_approx);
  std::vector<int> next_distances(n_approx);

  for (int step = 0; step < 5; ++step) {
    for (std::size_t i = 0; i < n_approx; ++i) {
      int max_distance = 0;
      for (std::size_t j = 0; j < i; ++j) {
        int candidate =
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

void Flight::optimize(const ObjectiveFunction &objective_function,
                      const BoundingFunction &bound_function) {
  double current_score = 0;

  std::priority_queue<CandidateTree> candidates;
  candidates.emplace();

  while (!candidates.empty()) {
    CandidateTree node = candidates.top();
    candidates.pop();

    if (node.is_single_candidate()) {
      current_score = objective_function(node);
      if (current_score > this->score) {
        this->score = current_score;
      }
    } else {
      for (auto &&child : node.branch()) {
        if (bound_function(child) >= this->score) {
          candidates.push(child);
        }
      }
    }
  }
}

/* CandidateTree */

CandidateTree::CandidateTree() { std::cout << "yelp" << std::endl; }

bool CandidateTree::is_single_candidate() { return false; }

std::vector<CandidateTree> CandidateTree::branch() {
  std::vector<CandidateTree> branches = {};
  return branches;
}

/* FreeObjectiveFunction */

double FreeObjectiveFunction::operator()(const CandidateTree &node) const {
  (void)node;
  return 0;
}

/* FreeBoundingFunction */

double FreeBoundingFunction::operator()(const CandidateTree &node) const {
  (void)node;
  return 0;
}