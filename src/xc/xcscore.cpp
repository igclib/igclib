#include <igclib/pointcollection.hpp>
#include <igclib/xcscore.hpp>
#include <nlohmann/json.hpp>

XCScore::XCScore() {
  this->style = xc_style::UNDEFINED;
  this->distance = 0;
  this->score = 0;
}

XCScore::XCScore(xc_style &style, double distance, double score) {
  this->style = style;
  this->distance = distance;
  this->score = score;
}

// Returns the JSON serialization of an XCScore
nlohmann::json XCScore::serialize() const {
  std::string xc_style;
  switch (this->style) {
  case xc_style::THREE_POINTS:
    xc_style = "Free distance over three points";
    break;
  case xc_style::FLAT_TRIANGLE:
    xc_style = "Flat triangle";
    break;
  case xc_style::FAI_TRIANGLE:
    xc_style = "FAI Triangle";
    break;
  default:
    xc_style = "Undefined";
    break;
  }
  return {{"style", xc_style},
          {"distance", this->distance / 1000},
          {"score", this->score}};
}

// Compares two candidates based on their score
bool Candidate::operator<(const Candidate &c) const {
  return score.score < c.score.score;
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

  this->before = std::vector<GeoPoint>{*p.begin()};
  this->after = std::vector<GeoPoint>{*(p.end() - 1)};
  this->closed = false; // to prevent use of undefined value in is_closed()
  this->closed = is_closed(2000); // TODO check closed condition FAI
  this->score = max_score();
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
        legs = {p1.distance(p2), p2.distance(p3), p3.distance(p1)};
        current_distance = std::accumulate(legs.begin(), legs.end(), 0);
        min_leg = *std::min_element(legs.begin(), legs.end());

        if (this->closed) {
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

std::pair<Candidate, Candidate> Candidate::branch() const {
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

  // split the largest group in half
  std::pair<PointCollection, PointCollection> split_pair =
      this->groups[index_largest_group].split();

  // if group being split is on the edge of the groups, update before and
  // after sets accordingly
  std::vector<GeoPoint> new_before = this->before;
  std::vector<GeoPoint> new_after = this->after;
  if (index_largest_group == 0) {
    new_before.insert(new_before.end(), split_pair.first.begin(),
                      split_pair.first.end());
  } else if (index_largest_group == 2) {
    new_after.insert(new_after.end(), split_pair.second.begin(),
                     split_pair.second.end());
  }

  // create the new candidates
  std::vector<PointCollection> g1 = this->groups;
  g1.erase(g1.begin() + index_largest_group);
  g1.push_back(split_pair.first);

  std::vector<PointCollection> g2 = this->groups;
  g2.erase(g2.begin() + index_largest_group);
  g2.push_back(split_pair.second);

  Candidate c1(g1, this->before, new_after, this->closed);
  Candidate c2(g2, new_before, this->after, this->closed);
  return std::make_pair(c1, c2);
}

Candidate::Candidate(std::vector<PointCollection> groups,
                     std::vector<GeoPoint> before, std::vector<GeoPoint> after,
                     bool already_closed) {
  this->before = before;
  this->after = after;
  this->groups = groups;
  this->closed = already_closed ? true : is_closed(0.005 * score.distance);
  this->score = this->max_score();
}

bool Candidate::is_closed(double tolerance) const {
  if (this->closed) {
    return true;
  }
  for (const GeoPoint &p1 : this->before) {
    for (const GeoPoint &p2 : this->after) {
      if (p1.distance(p2) < tolerance) {
        return true;
      }
    }
  }
  return false;
}