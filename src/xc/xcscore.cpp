#include <igclib/pointcollection.hpp>
#include <igclib/xcscore.hpp>
#include <nlohmann/json.hpp>

nlohmann::json XCScore::serialize() const {
  return {{"style", this->style},
          {"distance", this->distance},
          {"score", this->score}};
}

bool Candidate::operator<(const Candidate &c) const {
  return this->score.score < c.score.score;
}

bool Candidate::is_solution() const {
  for (const PointGroup &c : this->groups) {
    if (c.size() > 1) {
      return false;
    }
  }
  return true;
}

PointGroup::PointGroup(const linestring_t &l) { this->linestring = l; }

Candidate::Candidate(const linestring_t &points) {
  size_t group_size = points.size() / 3;
  for (int i = 0; i < 3; ++i) {
    size_t split_start = i * group_size;
    size_t split_stop = (i + 1) * group_size;
    linestring_t l =
        linestring_t(points.begin() + split_start, points.begin() + split_stop);
    this->groups.push_back(l);
  }
  this->score = max_score();
}

XCScore Candidate::max_score() const { return XCScore{}; }

std::pair<Candidate, Candidate> Candidate::branch() const {
  // find the largest group
  size_t index_largest_group;
  size_t max_size = 0;
  for (int i = 0; i < 3; ++i) {
    if (this->groups[i].size() > max_size) {
      index_largest_group = i;
      max_size = this->groups[i].size();
    }
  }

  // split the largest group in half
  std::pair<PointGroup, PointGroup> p =
      this->groups[index_largest_group].split();

  // if group being split is the first or the side, update the before and the
  // after set accordingly
  std::vector<GeoPoint> new_before = this->before;
  std::vector<GeoPoint> new_after = this->after;
  if (index_largest_group == 0) {
    new_before.insert(new_before.end(), p.first.begin(), p.first.end());
  } else if (index_largest_group == 2) {
    new_before.insert(new_after.end(), p.second.begin(), p.second.end());
  }

  // create the new candidates
  std::vector<PointGroup> p1 = this->groups;
  p1.erase(p1.begin() + index_largest_group);
  p1.push_back(p.first);

  std::vector<PointGroup> p2 = this->groups;
  p2.erase(p2.begin() + index_largest_group);
  p2.push_back(p.second);

  Candidate c1(p1, new_before, new_after);
  Candidate c2(p2, new_before, new_after);
  return std::make_pair(c1, c2);
}

std::pair<PointGroup, PointGroup> PointGroup::split() const {
  size_t const half_size = this->linestring.size() / 2;
  linestring_t l1(this->linestring.begin(),
                  this->linestring.begin() + half_size);
  linestring_t l2(this->linestring.begin() + half_size, this->linestring.end());
  return std::make_pair(l1, l2);
}

Candidate::Candidate(std::vector<PointGroup> groups,
                     std::vector<GeoPoint> before,
                     std::vector<GeoPoint> after) {
  this->before = before;
  this->after = after;
  this->groups = groups;
  this->score = max_score();
}