#include <boost/filesystem.hpp>
#include <cpr/cpr.h>
#include <fstream>
#include <igclib/airspace.hpp>
#include <igclib/cache.hpp>
#include <igclib/flight.hpp>
#include <igclib/geopoint.hpp>
#include <igclib/json.hpp>
#include <igclib/logging.hpp>
#include <igclib/pointcollection.hpp>
#include <igclib/time.hpp>
#include <igclib/util.hpp>
#include <igclib/xcopt.hpp>
#include <iostream>
#include <queue>
#include <stdexcept>
#include <string>

Flight::Flight(const std::string &igc_file) {
  // read and parse igc file
  std::ifstream f;
  f.open(igc_file);
  if (!f.is_open()) {
    const std::string error = "could not open file '" + igc_file + "'";
    throw std::runtime_error(error);
  }

  this->m_file_name =
      boost::filesystem::path(igc_file).filename().stem().string();
  this->m_pilot_name = "unknown pilot";

  std::string line;
  std::size_t lineno = 0;
  while (std::getline(f, line)) {
    ++lineno;
    try {
      switch (line[0]) {
      case 'H':
        // header records
        process_H_record(line);
        break;
      case 'B':
        // fix records
        process_B_record(line);
      default:
        // other records, not used yet
        break;
      }
    } catch (const std::runtime_error &err) {
      const std::string newerr = std::string(err.what()) + " at " + igc_file +
                                 ":" + std::to_string(lineno);
      throw std::runtime_error(newerr);
    }
  }

  // throw if no useful info could be parsed
  if (this->size() == 0) {
    const std::string err = igc_file + " is empty";
    throw std::runtime_error(err);
  }
}

// extracts pilot name and time offset
void Flight::process_H_record(const std::string &record) {
  const std::string record_code = record.substr(2, 3);
  if (record_code == "PLT") {
    size_t delim = record.find(':') + 1;
    this->m_pilot_name = record.substr(delim);
    util::trim(this->m_pilot_name);
  } else if (record_code == "TZO") {
    size_t delim = record.find(':') + 1;
    this->m_timezone_offset = IGCTimeOffset(record.substr(delim));
  }
}

void Flight::process_B_record(const std::string &record) {
  IGCTime &&t(record);
  IGCPoint &&p(record);
  try {
    if (!this->m_timezone_offset.zero()) {
      if (this->m_timezone_offset.is_negative()) {
        t -= this->m_timezone_offset;
      } else {
        t += this->m_timezone_offset;
      }
    }
    this->m_points.insert(t, p);
  } catch (const std::runtime_error &err) {
    logging::debug({err.what()}); // todo handle time overflow errors separately
  }
}

// Returns the JSON serialization of a Flight
json Flight::to_json() const {
  json j = {{"pilot", this->m_pilot_name},
            {"file", this->m_file_name},
            {"infractions", {}}};

  // airspace infractions
  for (const auto &infraction : this->m_infractions) {
    j["infractions"][infraction.first->name()] = infraction.first->to_json();
    j["infractions"][infraction.first->name()]["points"] =
        infraction.second.to_json();
  }

  j["xc_info"] = this->m_xcinfo.to_json();

  return j;
}

void Flight::save(const std::string &out) const {
  json j = this->to_json();

  if (out == "-" || out.empty()) {
    std::cout << j.dump(4) << std::endl;
  } else {
    std::ofstream f;
    f.open(out);
    if (!f.is_open()) {
      const std::string error = "could not open file '" + out + "'";
      throw std::runtime_error(error);
    }
    f << j.dump(4);
  }
}

void Flight::validate(const Airspace &airspace) {
  bool is_agl_validable = false;

  // retrieve the ground altitude for each point of the flight
  if (airspace.needs_agl_checking > 0) {
    if (getenv("ELEVATION_API_KEY")) {
      std::string key = getenv("ELEVATION_API_KEY");
      std::string api = "https://geolocalisation.ffvl.fr/elevation?key=" + key;
      json data = this->m_points.latlon();
      cpr::Body body(data.dump());
      cpr::Session session;
      session.SetVerifySsl(false);
      session.SetUrl(api);
      session.SetBody(body);
      logging::debug({"Airspace checking requires ground relative data, trying "
                      "to call elevation API"});
      cpr::Response r = session.Post();
      if (r.status_code == 200) {
        logging::debug({"Got API response"});
        data = json::parse(r.text);
        std::vector<double> altitudes = data.get<std::vector<double>>();
        if (this->m_points.set_agl(altitudes)) {
          is_agl_validable = true;
        }
      } else if (r.status_code == 400) {
        logging::warning({"Missing or invalid API key for the elevation API"});
      } else {
        logging::warning({"Elevation service could not be reached"});
      }
    } else {
      logging::warning(
          {"This airspace file contains",
           std::to_string(airspace.needs_agl_checking),
           "zones which need to be checked against ground altitude of the "
           "flight, but the environment variable 'ELEVATION_API_KEY' is not "
           "set. These zones will not be considered."});
    }
  }

  // then validate intersections
  airspace.infractions(this->m_points, this->m_infractions, is_agl_validable);
}

void Flight::compute_score() {
  // Inspired by by Ondrej Palkovsky
  // http://www.penguin.cz/~ondrap/algorithm.pdf
  double lower_bound = this->heuristic_free();
  std::cout << "Freeflight heuristic : " << lower_bound << std::endl;

  // double triangle_score = this->optimize_xc<TriangleCandidateTree>(0);
  // std::cout << "Triangle score : " << triangle_score << std::endl;

  // double fai_score = this->optimize_xc<FAICandidateTree>(triangle_score);
  // std::cout << "FAI score : " << fai_score << std::endl;

  // lower_bound = this->optimize_xc<FreeCandidateTree>(lower_bound);
  // std::cout << "Freeflight score : " << lower_bound << std::endl;

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
      std::cerr << "Score : " << candidate_score << std::endl;
      if (candidate_score > lower_bound) {
        best_score = candidate_score;
        lower_bound = best_score;
      }
    } else {
      for (auto &child : node.branch(*this)) {
        double node_bound = child.bound(*this);
        if (node_bound >= lower_bound) {
          std::cerr << "Bound : " << node_bound << std::endl;
          child.set_score(node_bound);
          candidates.push(child);
        }
      }
    }
  }
  return best_score;
}

// dynamic algorithm to compute the longest flight distance through 5 points
double Flight::heuristic_free() const {
  // number of points used for tracklog approximation
  const std::size_t n_approx = 100;
  const int step_ratio = this->m_points.size() / n_approx;
  std::size_t n_steps = 4;
  double candidate;
  double max_distance;

  std::array<double, n_approx> previous_distances{};
  std::array<double, n_approx> next_distances{};

  // n_steps must be number of points - 1
  for (std::size_t step = 0; step < n_steps; ++step) {
    for (std::size_t i = 0; i < n_approx; ++i) {
      max_distance = 0;
      for (std::size_t j = 0; j < i; ++j) {
        candidate = previous_distances.at(j) +
                    this->m_points.at(i * step_ratio)
                        ->distance(*this->m_points.at(j * step_ratio));
        if (candidate > max_distance) {
          max_distance = candidate;
        }
      }
      next_distances.at(i) = max_distance;
    }
    previous_distances = next_distances;
  }

  return *std::max_element(next_distances.begin(), next_distances.end());
}

/** accessors **/

const std::string &Flight::pilot() const { return this->m_pilot_name; }
const std::string &Flight::id() const { return this->m_file_name; }

const std::shared_ptr<GeoPoint> Flight::at(std::size_t index) const {
  return this->m_points.at(index);
}

const std::shared_ptr<GeoPoint> Flight::at(const Time &time) const {
  return this->m_points.at(time);
}

std::size_t Flight::size() const { return this->m_points.size(); }

double Flight::max_diagonal(std::pair<std::size_t, std::size_t> p) const {
  return this->m_points.max_diagonal(p.first, p.second);
}

std::array<GeoPoint, 4>
Flight::bbox(std::pair<std::size_t, std::size_t> p) const {
  return this->m_points.bbox(p.first, p.second);
}