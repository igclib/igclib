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

  std::vector<std::size_t> v_points;
  std::vector<std::pair<std::size_t, std::size_t>> v_boxes;

  double score() const { return this->m_score; }
  void set_score(double s) { this->m_score = s; }

private:
  double m_score = -1;
};

/* Objective Functions*/

class ObjectiveFunction {
public:
  ObjectiveFunction(){};
  virtual double operator()(const CandidateTree &node,
                            const Flight &flight) const = 0;
};

class FreeObjectiveFunction : public ObjectiveFunction {
public:
  FreeObjectiveFunction(){};
  virtual double operator()(const CandidateTree &node,
                            const Flight &flight) const;
};

class TriangleObjectiveFunction : public ObjectiveFunction {
public:
  TriangleObjectiveFunction(){};
  virtual double operator()(const CandidateTree &node,
                            const Flight &flight) const;
};

/* Bounding Functions*/

class BoundingFunction {
public:
  BoundingFunction(){};
  virtual double operator()(CandidateTree &node,
                            const Flight &flight) const = 0;
};

class FreeBoundingFunction : public BoundingFunction {
public:
  FreeBoundingFunction(){};
  virtual double operator()(CandidateTree &node, const Flight &flight) const;
};

class FAIBoundingFunction : public BoundingFunction {
public:
  FAIBoundingFunction(){};
  virtual double operator()(CandidateTree &node, const Flight &flight) const;
};

class TriangleBoundingFunction : public BoundingFunction {
public:
  TriangleBoundingFunction(){};
  virtual double operator()(CandidateTree &node, const Flight &flight) const;
};