#include <igclib/candidate.hpp>
#include <igclib/pointcollection.hpp>
#include <igclib/pointset.hpp>
#include <igclib/xcscore.hpp>
#include <vector>

bool Candidate::operator>(double min_score) const {
  return this->score.score > min_score;
}

bool Candidate::operator<(const Candidate &c) const {
  return (this->score < c.score);
}

bool Candidate::is_solution() const {
  for (const PointSet &p : this->sets) {
    if (p.can_be_split()) {
      return false;
    }
  }
  return true;
}

// std::vector<Candidate>
// Candidate::branch(const PointCollection &tracklog) const {
//  (void)tracklog;
//  return std::vector<Candidate>{};
//}

Candidate::Candidate(const PointCollection &tracklog) {
  this->takeoff = tracklog[0];
  this->landing = tracklog[tracklog.size()];
  PointSet p(0, (int)tracklog.size(), tracklog.bbox());
  this->sets = std::vector<PointSet>(3, p);
  this->score = this->score_function();
}

XCScore Candidate::score_function() {
  XCScore fai;
  XCScore flat;
  double best_score = 0;
  double best_distance = 0;

  for (const GeoPoint &p1 : this->sets[0].bbox) {
    for (const GeoPoint &p2 : this->sets[1].bbox) {
      for (const GeoPoint &p3 : this->sets[2].bbox) {
      }
    }
  }

  return fai > flat ? fai : flat;
}