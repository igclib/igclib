#pragma once

#include <igclib/geopoint.hpp>
#include <igclib/pointcollection.hpp>
#include <igclib/pointset.hpp>
#include <igclib/xcscore.hpp>
#include <vector>

class Candidate {
public:
  Candidate(const PointCollection &tracklog);

  bool operator>(double min_score) const;
  bool operator<(const Candidate& c) const;

  std::vector<Candidate> branch(const PointCollection &tracklog) const;
  bool is_solution() const;

  XCScore score;

private:
  std::vector<PointSet> sets;
  GeoPoint takeoff;
  GeoPoint landing;
};