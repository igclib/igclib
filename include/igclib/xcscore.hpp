#pragma once

#include <igclib/geopoint.hpp>
#include <igclib/pointcollection.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

enum class xc_style { UNDEFINED, THREE_POINTS, FLAT_TRIANGLE, FAI_TRIANGLE };

class XCScore {

public:
  XCScore();
  XCScore(xc_style &style, double distance, double score);
  nlohmann::json serialize() const;

  xc_style style;
  double distance;
  double score;
};

class Candidate {

public:
  Candidate(const PointCollection &points);
  Candidate(std::vector<PointCollection> groups, std::vector<GeoPoint> before,
            std::vector<GeoPoint> after, bool already_closed);
  bool operator<(const Candidate &c) const;
  std::pair<Candidate, Candidate> branch() const;
  bool is_solution() const;
  bool is_closed(double tolerance) const;

  XCScore score;

private:
  XCScore max_score() const;
  bool closed;
  std::vector<PointCollection> groups;
  std::vector<GeoPoint> before;
  std::vector<GeoPoint> after;
};