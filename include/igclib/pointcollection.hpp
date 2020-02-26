#pragma once

#include <igclib/geopoint.hpp>
#include <igclib/time.hpp>
#include <iterator>
#include <unordered_map>
#include <vector>

class PointCollection {
  typedef std::vector<GeoPoint> geopoints_t;
  typedef std::unordered_map<Time, size_t> timepoints_t;

public:
  PointCollection();
  PointCollection(geopoints_t::const_iterator start,
                  geopoints_t::const_iterator end);
  void insert(const Time &t, const GeoPoint &p);
  const GeoPoint &operator[](const Time &t) const;
  const GeoPoint &operator[](std::size_t index) const;
  std::size_t size() const { return geopoints.size(); };
  geopoints_t bbox() const;
  geopoints_t bbox(int start, int end) const;
  double max_diagonal() const;
  double max_diagonal(int start, int end) const;
  std::vector<std::pair<double, double>> latlon() const;
  bool set_agl(const std::vector<double> &altitudes);

  // this allows to range iterate over PointCollection
  geopoints_t::const_iterator begin() const { return geopoints.begin(); };
  geopoints_t::const_iterator end() const { return geopoints.end(); };

private:
  geopoints_t geopoints;
  timepoints_t timepoints;
};