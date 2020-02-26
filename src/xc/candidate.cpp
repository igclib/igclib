#include <igclib/candidate.hpp>
#include <igclib/pointcollection.hpp>
#include <igclib/pointset.hpp>
#include <igclib/xcscore.hpp>
#include <vector>

bool Candidate::operator>(double min_score) const {
  return this->score.score > min_score;
}

bool Candidate::operator<(double min_score) const {
  return this->score.score < min_score;
}

bool Candidate::operator<(const Candidate &c) const {
  return (this->score < c.score);
}

bool Candidate::operator>(const Candidate &c) const {
  return (this->score > c.score);
}

bool Candidate::is_solution() const {
  for (const PointSet &p : this->sets) {
    if (p.can_be_split()) {
      return false;
    }
  }
  return true;
}

Candidate::Candidate(const PointCollection &tracklog) {
  this->takeoff = tracklog[0];
  this->landing = tracklog[tracklog.size() - 1];
  PointSet p(0, (int)tracklog.size() - 1, tracklog.bbox());
  this->sets = std::vector<PointSet>(3, p);
}

LineCandidate::LineCandidate(const GeoPoint takeoff, const GeoPoint landing,
                             const std::vector<PointSet> &sets) {
  this->takeoff = takeoff;
  this->landing = landing;
  this->sets = sets;
}

std::vector<LineCandidate>
LineCandidate::branch(const PointCollection &tracklog) const {
  // TODO replace this by max element with > operator on pointset
  double max_diagonal = 0;
  double current_diagonal = 0;
  int max_set_idx = 0;
  for (int i = 0; i < 3; i++) {
    current_diagonal =
        tracklog.max_diagonal(this->sets[i].idx_start, this->sets[i].idx_stop);
    if (current_diagonal > max_diagonal) {
      max_diagonal = current_diagonal;
      max_set_idx = i;
    }
  }

  // TODO replace this with set.split
  const PointSet &max_set = this->sets[max_set_idx];
  int half_point = (max_set.idx_start + max_set.idx_stop) / 2;

  std::vector<PointSet> first_set(this->sets);
  std::vector<PointSet> second_set(this->sets);

  // TODO resize adjacent sets too or subsequent points could be before in time
  // ?
  first_set[max_set_idx].idx_stop = half_point;
  first_set[max_set_idx].bbox = tracklog.bbox(max_set.idx_start, half_point);

  second_set[max_set_idx].idx_start = half_point;
  second_set[max_set_idx].bbox = tracklog.bbox(half_point, max_set.idx_stop);

  LineCandidate c1(this->takeoff, this->landing, first_set);
  LineCandidate c2(this->takeoff, this->landing, second_set);

  this->bound(c1.score);
  this->bound(c2.score);

  return {c1, c2};
}

void LineCandidate::bound(XCScore &score) const {
  double max_distance = 0;
  double current_distance = 0;

  for (const GeoPoint &p1 : this->sets[0].bbox) {
    for (const GeoPoint &p2 : this->sets[1].bbox) {
      for (const GeoPoint &p3 : this->sets[2].bbox) {
        current_distance = this->takeoff.distance(p1) + p1.distance(p2) +
                           p2.distance(p3) + p3.distance(this->landing);
        if (current_distance > max_distance) {
          max_distance = current_distance;
        }
      }
    }
  }

  score.distance = max_distance;
  score.score = max_distance;
  score.style = xc_style::THREE_POINTS;
}