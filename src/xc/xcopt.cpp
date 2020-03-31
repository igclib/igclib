#include <igclib/cache.hpp>
#include <igclib/util.hpp>
#include <igclib/xcopt.hpp>
#include <vector>

CandidateTree::CandidateTree(const CandidateTree &other)
    : v_points(other.v_points), v_boxes(other.v_boxes), m_score(other.m_score) {
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
  for (auto &&combination : util::product<GeoPoint>(bboxes)) {
    current_score = 0;
    previous_point = combination.at(0);
    for (std::size_t i = 1; i < combination.size(); i++) {
      // current_score += previous_point.distance(combination.at(i));
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

/*

double CandidateTree::bound_triangle(const Flight &flight) {
  std::vector<std::vector<GeoPoint>> bboxes;
  for (std::size_t i = 0; i < this.v_points.size(); i++) {
    auto box = flight.bbox(this.v_boxes.at(i));
    bboxes.insert(bboxes.end(), this.v_points.at(i), box);
  }

  auto closing_boxes = {bboxes.front(), bboxes.back()};
  bboxes.pop_back();
  bboxes.erase(bboxes.begin());
  double best_closing = std::numeric_limits<double>::max();
  double current_closing = 0;

  for (auto &&combination : util::product<GeoPoint>(closing_boxes)) {
    // current_closing = combination.front().distance(combination.back());
    current_closing = cache::distance(combination.front(), combination.back());
    if (current_closing < best_closing) {
      best_closing = current_closing;
    }
  }

  double best_score = 0;
  double current_fai_score = 0;
  double current_triangle_score = 0;
  double min_leg = 0;
  double c_fai = 0;
  std::vector<double> legs;
  for (auto &&combination : util::product<GeoPoint>(bboxes)) {
    legs.clear();
    // legs.push_back(combination.at(0).distance(combination.at(1)));
    // legs.push_back(combination.at(1).distance(combination.at(2)));
    // legs.push_back(combination.at(2).distance(combination.at(0)));
    legs.push_back(cache::distance(combination.at(0), combination.at(1)));
    legs.push_back(cache::distance(combination.at(1), combination.at(2)));
    legs.push_back(cache::distance(combination.at(2), combination.at(0)));
    current_triangle_score =
        std::accumulate(legs.begin(), legs.end(), 0) - best_closing;
    min_leg = *std::min_element(legs.begin(), legs.end());
    c_fai = (min_leg / 0.28) - best_closing;
    current_fai_score = std::min(current_triangle_score, c_fai);

    if (current_triangle_score > best_score) {
      best_score = current_triangle_score;
    } else if (current_fai_score > best_score) {
      // best_score = current_fai_score;
    }
  }

  best_score *= 1.2;
  return best_score;
}
*/