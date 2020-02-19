#include <igclib/candidate.hpp>
#include <igclib/pointcollection.hpp>
#include <igclib/pointset.hpp>
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
  PointSet p(0, (int)tracklog.size(), 3, tracklog.bbox());
  this->sets.push_back(p);
}
