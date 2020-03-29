#pragma once

#include <igclib/airspace.hpp>
#include <igclib/geopoint.hpp>
#include <map>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

class CandidateTree;
class ObjectiveFunction;
class BoundingFunction;

class Flight {
  typedef std::vector<GeoPoint> geopoints_t;
  typedef std::map<std::string, geopoints_t> infractions_t;

public:
  Flight(const std::string &igc_file);
  void save(const std::string &out) const;
  void validate(const Airspace &airspace);
  nlohmann::json serialize() const;
  void compute_score();
  geopoints_t bbox(std::pair<std::size_t, std::size_t> pair) const {
    return this->points.bbox(pair.first, pair.second);
  };
  double max_diagonal(std::pair<int, int> pair) const {
    return this->points.max_diagonal(pair.first, pair.second);
  };

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

class CandidateTree {
public:
  CandidateTree(std::vector<std::size_t> points,
                std::vector<std::pair<std::size_t, std::size_t>> boxes)
      : v_points(points), v_boxes(boxes){};
  CandidateTree(const CandidateTree &other)
      : v_points(other.v_points), v_boxes(other.v_boxes),
        m_score(other.m_score){};
  bool operator<(const CandidateTree &other) const {
    return this->m_score < other.m_score;
  };

  bool is_single_candidate();
  std::vector<CandidateTree> branch(const Flight &flight);

  std::vector<std::size_t> v_points;
  std::vector<std::pair<std::size_t, std::size_t>> v_boxes;

  double score() const { return this->m_score; }
  void set_score(double s) { this->m_score = s; }

private:
  double m_score = -1;
};

/* Objective Functions*/

class ObjectiveFunction {
public:
  ObjectiveFunction(){};
  virtual double operator()(const CandidateTree &node,
                            const Flight &flight) const = 0;
};

class FreeObjectiveFunction : public ObjectiveFunction {
public:
  FreeObjectiveFunction(){};
  virtual double operator()(const CandidateTree &node,
                            const Flight &flight) const;
};

/* Bounding Functions*/

class BoundingFunction {
public:
  BoundingFunction(){};
  virtual double operator()(CandidateTree &node,
                            const Flight &flight) const = 0;
};

class FreeBoundingFunction : public BoundingFunction {
public:
  FreeBoundingFunction(){};
  virtual double operator()(CandidateTree &node, const Flight &flight) const;
};

class FAIBoundingFunction : public BoundingFunction {
public:
  FAIBoundingFunction(){};
  virtual double operator()(CandidateTree &node, const Flight &flight) const;
};

class TriangleBoundingFunction : public BoundingFunction {
public:
  TriangleBoundingFunction(){};
  virtual double operator()(CandidateTree &node, const Flight &flight) const;
};