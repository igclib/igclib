#include <fstream>
#include <igclib/airspace.hpp>
#include <igclib/flight.hpp>
#include <igclib/geopoint.hpp>
#include <igclib/pointcollection.hpp>
#include <igclib/time.hpp>
#include <igclib/util.hpp>
#include <igclib/xcscore.hpp>
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

  compute_score();

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

  for (auto &infraction : this->infractions) {
    j["infractions"][infraction.first];
    for (auto &p : infraction.second) {
      j["infractions"][infraction.first].push_back(p.serialize());
    }
  }

  j["xc_info"] = this->score.serialize();

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
  airspace.infractions(this->points, this->infractions);
}

void Flight::compute_score() {
  // Algorithm designed by Ondrej Palkovsky
  // http://www.penguin.cz/~ondrap/algorithm.pdf

  std::priority_queue<Candidate> candidates;
  Candidate initial_guess(this->points);
  candidates.push(initial_guess);

  while (!candidates.top().is_solution()) {
    std::pair<Candidate, Candidate> branches = candidates.top().branch();
    candidates.pop();
    candidates.push(branches.first);
    candidates.push(branches.second);
  }

  this->score = candidates.top().score;
}