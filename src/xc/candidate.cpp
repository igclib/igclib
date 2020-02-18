#include <igclib/candidate.hpp>
#include <igclib/geopoint.hpp>
#include <igclib/pointcollection.hpp>
#include <vector>

// Compares two candidates based on their score
bool Candidate::operator<(const Candidate &c) const {
  return this->score.score < c.score.score;
}

bool Candidate::operator>(const HeuristicCandidate &c) const {
  return this->score.score > c.score.score;
}

// A candidate is solution only if its three groups contains at most one
// GeoPoint
bool Candidate::is_solution() const {
  for (const PointCollection &c : this->groups) {
    if (c.size() > 1) {
      return false;
    }
  }
  return true;
}

bool Candidate::is_closed(double tolerance) const {
  for (const GeoPoint &p1 : this->points_before) {
    for (const GeoPoint &p2 : this->points_after) {
      if (p1.distance(p2) < tolerance) {
        return true;
      }
    }
  }
  return false;
}

// Initial constructor creates the groups by splitting the points in three
// thirds. Before and after only contain the first and the last point as the
// groups represent all the remaining points.
Candidate::Candidate(const PointCollection &p) {
  size_t group_size = p.size() / 3;
  for (int i = 0; i < 3; ++i) {
    size_t split_start = i * group_size;
    size_t split_stop = (i + 1) * group_size;
    PointCollection group(p.begin() + split_start, p.begin() + split_stop);
    groups.push_back(group);
  }

  this->first_point = *p.begin();
  this->last_point = *(p.end() - 1);
  this->points_before = std::vector<GeoPoint>{this->first_point};
  this->points_after = std::vector<GeoPoint>{this->last_point};
  this->closed = is_closed(2000); // TODO check closed condition FAI
  this->score = max_score();
}

Candidate::Candidate(const Candidate &c) {
  this->first_point = c.first_point;
  this->last_point = c.last_point;
  this->points_before = c.points_before;
  this->points_after = c.points_after;
  this->groups = c.groups;
  this->closed = c.closed;
  this->score = c.score;
}

std::size_t Candidate::find_split() const {
  // find the group with the largest area
  size_t index_largest_group = 0;
  double max_size = 0;
  for (int i = 0; i < 3; ++i) {
    double group_area = this->groups[i].bbox_area();
    if (group_area > max_size) {
      index_largest_group = i;
      max_size = group_area;
    }
  }
  return index_largest_group;
}

std::pair<Candidate, Candidate> Candidate::branch() const {
  std::size_t to_split = this->find_split();

  // split the "largest" group in half
  auto split_pair = this->groups[to_split].split();

  // if group being split is on the edge of the groups, update before and
  // after sets accordingly
  std::vector<GeoPoint> new_before = this->points_before;
  std::vector<GeoPoint> new_after = this->points_after;
  if (to_split == 0) {
    new_before.insert(new_before.end(), split_pair.first.begin(),
                      split_pair.first.end());
  } else if (to_split == 2) {
    new_after.insert(new_after.end(), split_pair.second.begin(),
                     split_pair.second.end());
  }

  // TODO implement proper splitting
  // create the new candidates
  std::vector<PointCollection> g1 = this->groups;
  g1.erase(g1.begin() + to_split);
  g1.push_back(split_pair.first);

  std::vector<PointCollection> g2 = this->groups;
  g2.erase(g2.begin() + to_split);
  g2.push_back(split_pair.second);

  Candidate c1(*this);
  Candidate c2(*this);

  return std::make_pair(c1, c2);
}
HeuristicCandidate::HeuristicCandidate(const PointCollection &points) {
  (void)points;
  this->score = XCScore();
}

XCScore Candidate::max_score() const {
  double best_score = 0;
  double best_distance = 0;
  double current_distance = 0;
  double current_score = 0;
  double min_leg = 0;
  xc_style best_style = xc_style::UNDEFINED;
  xc_style current_style = xc_style::UNDEFINED;
  std::vector<GeoPoint> best_points;
  std::vector<double> legs;

  for (const GeoPoint &p1 : this->groups[0].bbox()) {
    for (const GeoPoint &p2 : this->groups[1].bbox()) {
      for (const GeoPoint &p3 : this->groups[2].bbox()) {
        if (this->closed) {
          legs = {p1.distance(p2), p2.distance(p3), p3.distance(p1)};
        } else {
          legs = {this->first_point.distance(p1), p1.distance(p2),
                  p2.distance(p3), p3.distance(this->last_point)};
        }
        current_distance = std::accumulate(legs.begin(), legs.end(), 0);

        if (this->closed) {
          min_leg = *std::min_element(legs.begin(), legs.end());
          if (min_leg / current_distance > 0.28) {
            current_score = 0.0014 * current_distance;
            current_style = xc_style::FAI_TRIANGLE;
          } else {
            current_score = 0.0012 * current_distance;
            current_style = xc_style::FLAT_TRIANGLE;
          }
        } else {
          current_score = 0.001 * current_distance;
          current_style = xc_style::THREE_POINTS;
        }

        if (current_score > best_score) {
          best_score = current_score;
          best_distance = current_distance;
          best_style = current_style;
          best_points = {p1, p2, p3};
        }
      }
    }
  }

  return XCScore(best_style, best_distance, best_score);
}
