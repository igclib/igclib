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
  CandidateTree &operator=(const CandidateTree &other);
  bool operator<(const CandidateTree &other) const {
    return this->m_score < other.m_score;
  };

  void set_score(double s) { this->m_score = s; };

  template <class T> std::vector<T> branch(const Flight &flight) const;
  virtual bool is_single_candidate() const;
  virtual double bound(const Flight &flight) const = 0;
  virtual double score(const Flight &flight) const = 0;

protected:
  std::vector<std::size_t> v_points;
  std::vector<std::pair<std::size_t, std::size_t>> v_boxes;
  double m_score = -1;
};

class FreeCandidateTree : public CandidateTree {
public:
  FreeCandidateTree(const Flight &flight) : CandidateTree(flight){};
  FreeCandidateTree(const CandidateTree &other) : CandidateTree(other){};
  virtual std::vector<FreeCandidateTree> branch(const Flight &flight) const;
  virtual double bound(const Flight &flight) const;
  virtual double score(const Flight &flight) const;
};

class TriangleCandidateTree : public CandidateTree {
public:
  TriangleCandidateTree(const Flight &flight) : CandidateTree(flight){};
  TriangleCandidateTree(const CandidateTree &other) : CandidateTree(other){};
  virtual std::vector<TriangleCandidateTree> branch(const Flight &flight) const;
  virtual double bound(const Flight &flight) const;
  virtual double score(const Flight &flight) const;
};

class FAICandidateTree : public CandidateTree {
public:
  FAICandidateTree(const Flight &flight) : CandidateTree(flight){};
  FAICandidateTree(const CandidateTree &other) : CandidateTree(other){};
  virtual std::vector<FAICandidateTree> branch(const Flight &flight) const;
  virtual double bound(const Flight &flight) const;
  virtual double score(const Flight &flight) const;
};