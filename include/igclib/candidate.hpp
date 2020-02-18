#pragma once

#include <igclib/geopoint.hpp>
#include <igclib/pointcollection.hpp>
#include <igclib/xcscore.hpp>
#include <vector>

class HeuristicCandidate {
public:
  HeuristicCandidate(const PointCollection &points);
  XCScore score;
};

class Candidate {

public:
  Candidate(const PointCollection &points);
  Candidate(const Candidate &c);
  bool operator<(const Candidate &c) const;
  bool operator>(const HeuristicCandidate &c) const;
  std::pair<Candidate, Candidate> branch() const;
  bool is_solution() const;

  XCScore score;

private:
  XCScore max_score() const;
  bool is_closed(double tolerance) const;
  std::size_t find_split() const;

  bool closed;
  std::vector<PointCollection> groups;
  GeoPoint first_point;
  GeoPoint last_point;
  std::vector<GeoPoint> points_before;
  std::vector<GeoPoint> points_after;
};
