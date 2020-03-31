#include <igclib/cache.hpp>
#include <igclib/util.hpp>
#include <igclib/xcopt.hpp>
#include <vector>

CandidateTree::CandidateTree(const CandidateTree &other)
    : v_points(other.v_points), v_boxes(other.v_boxes), m_score(other.m_score) {
}

CandidateTree &CandidateTree::operator=(const CandidateTree &other) {
  v_points = other.v_points;
  v_boxes = other.v_boxes;
  m_score = other.m_score;
  return *this;
}

CandidateTree::CandidateTree(const Flight &flight)
    : v_points{5}, v_boxes{{(std::size_t)0, (std::size_t)flight.size() - 1}} {}

bool CandidateTree::is_single_candidate() const {
  for (std::size_t i = 0; i < this->v_boxes.size(); i++) {
    auto box_size = this->v_boxes.at(i).second - this->v_boxes.at(i).first;
    if (box_size > 10) { // margin ?
      return false;
    }
  }
  return true;
}

template <class T>
std::vector<T> CandidateTree::branch(const Flight &flight) const {
  std::vector<T> branches = {};

  std::size_t picked_index = 0;
  std::size_t largest_box = 0;

  for (std::size_t i = 0; i < this->v_boxes.size(); i++) {
    // eager branching of box sharing points (DOESN'T WORK YET BECAUSE TRIES TO
    // SPLIT CLOSING POINTS)
    // if (this->v_points.at(i) > 1) {
    //  picked_index = i;
    //  break;
    //}
    // branching on box with largest diagonal
    auto box_size = flight.max_diagonal(this->v_boxes.at(i));
    if (box_size > largest_box) {
      picked_index = i;
      largest_box = box_size;
    }
  }

  auto n = this->v_points[picked_index];
  auto picked_box = this->v_boxes[picked_index];
  auto half_point = (picked_box.first + picked_box.second) / 2;

  for (std::size_t i = 0; i <= n; i++) {
    // all elements must be deep copied to not modify original objects
    auto first_half = std::make_pair(picked_box.first, half_point);
    auto second_half = std::make_pair(half_point, picked_box.second);

    T new_candidate(*this);
    new_candidate.m_score = -1; // reset score is is very important
    auto box_it = new_candidate.v_boxes.begin() + picked_index + 1;
    auto points_it = new_candidate.v_points.begin() + picked_index + 1;

    if (n - i > 0) {
      new_candidate.v_points.insert(points_it, n - i);
      new_candidate.v_boxes.insert(box_it, second_half);
    }
    if (i > 0) {
      new_candidate.v_points.at(picked_index) = i;
      new_candidate.v_boxes.at(picked_index) = first_half;
    } else {
      auto points_to_erase = new_candidate.v_points.begin() + picked_index;
      auto box_to_erase = new_candidate.v_boxes.begin() + picked_index;
      new_candidate.v_points.erase(points_to_erase);
      new_candidate.v_boxes.erase(box_to_erase);
    }

    branches.push_back(new_candidate);
  }

  return branches;
}

/* Free flight */

std::vector<FreeCandidateTree>
FreeCandidateTree::branch(const Flight &flight) const {
  return CandidateTree::branch<FreeCandidateTree>(flight);
}

double FreeCandidateTree::bound(const Flight &flight) const {
  std::vector<std::vector<GeoPoint>> bboxes;
  for (std::size_t i = 0; i < this->v_points.size(); i++) {
    auto box = flight.bbox(this->v_boxes.at(i));
    bboxes.insert(bboxes.end(), this->v_points.at(i), box);
  }

  double best_score = 0;
  double current_score = 0;
  GeoPoint previous_point;
  for (auto &combination : util::product<GeoPoint>(bboxes)) {
    current_score = 0;
    previous_point = combination.at(0);
    for (std::size_t i = 1; i < combination.size(); i++) {
      current_score += cache::distance(previous_point, combination.at(i));
      previous_point = combination.at(i);
    }
    if (current_score > best_score) {
      best_score = current_score;
    }
  }

  return best_score;
}

double FreeCandidateTree::score(const Flight &flight) const {
  return this->bound(flight);
}

/* Triangle flight */

std::vector<TriangleCandidateTree>
TriangleCandidateTree::branch(const Flight &flight) const {
  return CandidateTree::branch<TriangleCandidateTree>(flight);
}

double TriangleCandidateTree::bound(const Flight &flight) const {
  std::vector<std::vector<GeoPoint>> bboxes;
  for (std::size_t i = 0; i < this->v_points.size(); i++) {
    auto box = flight.bbox(this->v_boxes.at(i));
    bboxes.insert(bboxes.end(), this->v_points.at(i), box);
  }

  auto closing_boxes = {bboxes.front(), bboxes.back()};
  bboxes.pop_back();
  bboxes.erase(bboxes.begin());
  double best_closing = std::numeric_limits<double>::max();
  double current_closing = 0;

  for (auto &combination : util::product<GeoPoint>(closing_boxes)) {
    current_closing = cache::distance(combination.front(), combination.back());
    if (current_closing < best_closing) {
      best_closing = current_closing;
    }
  }

  double best_score = 0;
  double current_score = 0;
  for (auto &combination : util::product<GeoPoint>(bboxes)) {
    current_score = cache::distance(combination.at(0), combination.at(1));
    current_score += cache::distance(combination.at(1), combination.at(2));
    current_score += cache::distance(combination.at(2), combination.at(0));
    if (current_score > best_score) {
      best_score = current_score;
    }
  }

  return (best_score - best_closing);
}

double TriangleCandidateTree::score(const Flight &flight) const {
  GeoPoint start_point = flight.at(this->v_boxes.front().first);
  GeoPoint end_point = flight.at(this->v_boxes.back().first);
  double closing_distance = start_point.distance(end_point);
  double triangle_distance =
      flight.at(this->v_boxes.at(1).first)
          .distance(flight.at(this->v_boxes.at(2).first));
  triangle_distance += flight.at(this->v_boxes.at(2).first)
                           .distance(flight.at(this->v_boxes.at(3).first));
  triangle_distance += flight.at(this->v_boxes.at(3).first)
                           .distance(flight.at(this->v_boxes.at(1).first));
  if (closing_distance < 3000.0) {
    return 1.2 * triangle_distance;
  } else if (closing_distance < 0.05 * triangle_distance) {
    return 1.2 * (triangle_distance - closing_distance);
  } else {
    return 0;
  }
}

/* FAI flight */

std::vector<FAICandidateTree>
FAICandidateTree::branch(const Flight &flight) const {
  return CandidateTree::branch<FAICandidateTree>(flight);
}

double FAICandidateTree::bound(const Flight &flight) const {
  std::vector<std::vector<GeoPoint>> bboxes;
  for (std::size_t i = 0; i < this->v_points.size(); i++) {
    auto box = flight.bbox(this->v_boxes.at(i));
    bboxes.insert(bboxes.end(), this->v_points.at(i), box);
  }

  auto closing_boxes = {bboxes.front(), bboxes.back()};
  bboxes.pop_back();
  bboxes.erase(bboxes.begin());
  double best_closing = std::numeric_limits<double>::max();
  double current_closing = 0;

  for (auto &combination : util::product<GeoPoint>(closing_boxes)) {
    current_closing = cache::distance(combination.front(), combination.back());
    if (current_closing < best_closing) {
      best_closing = current_closing;
    }
  }

  double best_score = std::numeric_limits<double>::min();
  double current_score = 0;
  double min_leg = 0;
  double c_fai = 0;
  std::vector<double> legs;
  for (auto &combination : util::product<GeoPoint>(bboxes)) {
    legs.clear();
    legs.push_back(combination.at(0).distance(combination.at(1)));
    legs.push_back(combination.at(1).distance(combination.at(2)));
    legs.push_back(combination.at(2).distance(combination.at(0)));
    current_score = std::accumulate(legs.begin(), legs.end(), 0);
    min_leg = *std::min_element(legs.begin(), legs.end());
    c_fai = min_leg / 0.28 - best_closing;
    current_score = std::min(current_score, c_fai);
    if (current_score > best_score) {
      best_score = current_score;
    }
  }

  return best_score;
}

double FAICandidateTree::score(const Flight &flight) const {
  GeoPoint start_point = flight.at(this->v_boxes.front().first);
  GeoPoint end_point = flight.at(this->v_boxes.back().first);
  double closing_distance = start_point.distance(end_point);
  std::vector<double> legs;
  legs.push_back(flight.at(this->v_boxes.at(1).first)
                     .distance(flight.at(this->v_boxes.at(2).first)));
  legs.push_back(flight.at(this->v_boxes.at(2).first)
                     .distance(flight.at(this->v_boxes.at(3).first)));
  legs.push_back(flight.at(this->v_boxes.at(3).first)
                     .distance(flight.at(this->v_boxes.at(1).first)));
  double min_leg = *std::min_element(legs.begin(), legs.end());
  double triangle_distance = std::accumulate(legs.begin(), legs.end(), 0.0);
  if (min_leg >= 0.28 * triangle_distance) {
    if (closing_distance < 3000.0) {
      return 1.4 * triangle_distance;
    } else if (closing_distance < 0.05 * triangle_distance) {
      return 1.4 * (triangle_distance - closing_distance);
    }
  }
  return 0;
}
