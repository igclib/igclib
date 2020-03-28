#pragma once

#include <igclib/airspace.hpp>
#include <igclib/geopoint.hpp>
#include <map>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

class CandidateTree {
public:
  CandidateTree();
  bool operator<(const CandidateTree &other) const {
    (void)other;
    return true;
  }
  bool is_single_candidate();
  std::vector<CandidateTree> branch();
};

class ObjectiveFunction {
public:
  ObjectiveFunction(){};
  virtual double operator()(const CandidateTree &node) const = 0;
};

class FreeObjectiveFunction : public ObjectiveFunction {
public:
  FreeObjectiveFunction(){};
  virtual double operator()(const CandidateTree &node) const;
};

class BoundingFunction {
public:
  BoundingFunction(){};
  virtual double operator()(const CandidateTree &node) const = 0;
};

class FreeBoundingFunction : public BoundingFunction {
public:
  FreeBoundingFunction(){};
  virtual double operator()(const CandidateTree &node) const;
};

class Flight {
  typedef std::vector<GeoPoint> geopoints_t;
  typedef std::map<std::string, geopoints_t> infractions_t;

public:
  Flight(const std::string &flight_file);
  void save(const std::string &out) const;
  void validate(const Airspace &airspace);
  nlohmann::json serialize() const;
  void compute_score();

private:
  void process_H_record(const std::string &record);
  void process_B_record(const std::string &record);
  double heuristic_score();
  void optimize(const ObjectiveFunction &objective_function,
                const BoundingFunction &bound_function);

  int time_zone_offset = 0;
  double score = 0;
  std::string pilot_name = "Unknown pilot";
  PointCollection points;
  infractions_t infractions;
};