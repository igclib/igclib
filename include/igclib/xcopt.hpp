#include <igclib/flight.hpp>
#include <vector>

class Flight;

class CandidateTree {
public:
  CandidateTree(const Flight &flight);
  CandidateTree(std::vector<std::size_t> points,
                std::vector<std::pair<std::size_t, std::size_t>> boxes)
      : v_points(points), v_boxes(boxes){};
  CandidateTree(const CandidateTree &other);
  bool operator<(const CandidateTree &other) const {
    return this->m_score < other.m_score;
  };

  bool is_single_candidate();
  std::vector<CandidateTree> branch(const Flight &flight) const;
  double bound(const Flight &flight);
  double score() const { return this->m_score; }

private:
  double bound_free(const Flight &flight);
  double bound_triangle(const Flight &flight);
  std::vector<std::size_t> v_points;
  std::vector<std::pair<std::size_t, std::size_t>> v_boxes;
  double m_score = -1;
};