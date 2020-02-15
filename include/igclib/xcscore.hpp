#pragma once

#include <igclib/geopoint.hpp>
#include <igclib/pointcollection.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

class XCScore {
public:
  nlohmann::json serialize() const;

  double distance;
  std::string style;
  double score;
};

class PointGroup {

public:
  PointGroup();
  PointGroup(const linestring_t &points);
  std::pair<PointGroup, PointGroup> split() const;
  linestring_t::const_iterator begin() const {
    return this->linestring.begin();
  };
  linestring_t::const_iterator end() const { return this->linestring.end(); };
  size_t size() const { return this->linestring.size(); };
  linestring_t linestring;
};

class Candidate {
public:
  Candidate(const linestring_t &points);
  Candidate(std::vector<PointGroup> groups, std::vector<GeoPoint> before,
            std::vector<GeoPoint> after);
  bool operator<(const Candidate &c) const;
  std::pair<Candidate, Candidate> branch() const;
  XCScore max_score() const;

  bool is_solution() const;
  XCScore score;

private:
  std::vector<PointGroup> groups;
  std::vector<GeoPoint> before;
  std::vector<GeoPoint> after;
};