#pragma once

#include <igclib/geopoint.hpp>
#include <igclib/pointcollection.hpp>
#include <igclib/pointset.hpp>
#include <igclib/xcscore.hpp>
#include <vector>

class Candidate {
public:
  Candidate(const PointCollection &tracklog);
  Candidate(){};

  bool operator>(double min_score) const;
  bool operator<(double min_score) const;
  bool operator>(const Candidate &c) const;
  bool operator<(const Candidate &c) const;
  bool is_solution() const;

  XCScore score;

protected:
  std::vector<PointSet> sets;
  GeoPoint takeoff;
  GeoPoint landing;
};

class LineCandidate : public Candidate {
public:
  LineCandidate(const PointCollection &tracklog) : Candidate(tracklog){};
  LineCandidate(const GeoPoint takeoff, const GeoPoint landing,
                const std::vector<PointSet> &sets);
  std::vector<LineCandidate> branch(const PointCollection &tracklog) const;

private:
  void bound(XCScore &score) const;
};

class TriangleCandidate : public Candidate {
public:
  TriangleCandidate(const PointCollection &tracklog) : Candidate(tracklog){};
  std::vector<TriangleCandidate> branch(const PointCollection &tracklog) const;

private:
  void bound(XCScore &score) const;
};