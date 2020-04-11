#include <cpr/cpr.h>
#include <fstream>
#include <igclib/airspace.hpp>
#include <igclib/cache.hpp>
#include <igclib/flight.hpp>
#include <igclib/geopoint.hpp>
#include <igclib/pointcollection.hpp>
#include <igclib/time.hpp>
#include <igclib/util.hpp>
#include <igclib/xcopt.hpp>
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

  for (const auto &infraction : this->infractions) {
    j["infractions"][infraction.first.name()] = infraction.first.to_json();
    for (const GeoPoint &p : infraction.second) {
      std::string time(this->points.find_time(p).to_string());
      j["infractions"][infraction.first.name()]["points"][time] = p.serialize();
    }
  }

  j["xc_info"] = this->xcinfo.serialize();

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
      cpr::Body body(data.dump());
      cpr::Session session;
      session.SetVerifySsl(false);
      session.SetUrl(api);
      session.SetBody(body);
      std::cerr << "Retrieving flight altitude ... ";
      cpr::Response r = session.Post();
      if (r.status_code == 200) {
        std::cerr << "done." << std::endl;
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
  double lower_bound = this->heuristic_free();
  std::cout << "Freeflight heuristic : " << lower_bound << std::endl;

  // lower_bound = this->optimize_xc<FreeCandidateTree>(lower_bound);
  std::cout << "Freeflight score : " << lower_bound << std::endl;

  double triangle_score = this->optimize_xc<TriangleCandidateTree>(0);
  std::cout << "Triangle score : " << triangle_score << std::endl;

  double fai_score = this->optimize_xc<FAICandidateTree>(triangle_score);
  std::cout << "FAI score : " << fai_score << std::endl;

  // compute final score and assign to flight
  cache::print_stats();
}

template <class T> double Flight::optimize_xc(double lower_bound) {
  double best_score = 0;
  std::priority_queue<T> candidates;
  candidates.emplace(*this);

  while (!candidates.empty()) {
    T node = candidates.top();
    candidates.pop();
    if (node.is_single_candidate()) {
      double candidate_score = node.score(*this);
      if (candidate_score > lower_bound) {
        best_score = candidate_score;
        lower_bound = best_score;
      }
    } else {
      for (auto &child : node.branch(*this)) {
        double node_bound = child.bound(*this);
        if (node_bound >= lower_bound) {
          child.set_score(node_bound);
          candidates.push(child);
        }
      }
    }
  }
  return best_score;
}

double Flight::heuristic_free() const {
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
