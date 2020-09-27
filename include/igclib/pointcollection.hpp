#pragma once

#include <igclib/geopoint.hpp>
#include <igclib/json.hpp>
#include <igclib/time.hpp>
#include <map>
#include <memory>
#include <vector>

class PointCollection {
  typedef std::vector<std::shared_ptr<GeoPoint>> geopoints_t;
  typedef std::map<Time, std::shared_ptr<GeoPoint>> timepoints_t;

public:
  PointCollection() : m_timepoints{}, m_geopoints{} {};
  void insert(const Time &t, const GeoPoint &p);
  void insert(const std::pair<const Time, std::shared_ptr<GeoPoint>> &p);
  const std::shared_ptr<GeoPoint> at(std::size_t idx) const;
  const std::shared_ptr<GeoPoint> at(const Time &t) const;
  std::size_t size() const { return this->m_geopoints.size(); };
  const std::array<GeoPoint, 4> bbox() const;
  const std::array<GeoPoint, 4> bbox(std::size_t start, std::size_t end) const;
  double max_diagonal() const;
  double max_diagonal(int start, int end) const;
  const std::vector<std::pair<double, double>> latlon() const;
  bool set_agl(const std::vector<double> &altitudes);
  json to_json() const;
  auto cbegin() const { return m_timepoints.cbegin(); }
  auto cend() const { return m_timepoints.cend(); }

  // range iterate over geopoints in PointCollection
  const geopoints_t &geopoints() const { return this->m_geopoints; }
  const timepoints_t &timepoints() const { return this->m_timepoints; }

private:
  timepoints_t m_timepoints;
  geopoints_t m_geopoints;
};