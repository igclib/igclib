#include <igclib/pointcollection.hpp>
#include <iostream>
#include <iterator>
#include <vector>

PointCollection::PointCollection() {
  this->geopoints = {};
  this->timepoints = {};
}

PointCollection ::PointCollection(geopoints_t::const_iterator start,
                                  geopoints_t::const_iterator end) {
  this->geopoints = geopoints_t(start, end);
}

void PointCollection::insert(const Time &t, const GeoPoint &p) {
  // is this really safe ? do some testing
  // TODO shared pointers rather than index
  timepoints.emplace(t, geopoints.size());
  geopoints.push_back(p);
}

const GeoPoint &PointCollection::operator[](const Time &t) const {
  std::size_t index = timepoints.at(t);
  return geopoints.at(index);
}

const GeoPoint &PointCollection::operator[](std::size_t index) const {
  return geopoints.at(index);
}

const GeoPoint &PointCollection::at(std::size_t index) const {
  return geopoints.at(index);
}

std::vector<GeoPoint> PointCollection::bbox() const {
  double min_lat = 90.0;
  double max_lat = -90.0;
  double min_lon = 180.0;
  double max_lon = -180;
  for (const GeoPoint &p : geopoints) {
    min_lat = std::min(p.lat, min_lat);
    max_lat = std::max(p.lat, max_lat);
    min_lon = std::min(p.lon, min_lon);
    max_lon = std::max(p.lon, max_lon);
  }
  std::vector<GeoPoint> bbox = {
      GeoPoint(min_lat, min_lon, 0, 0),
      GeoPoint(min_lat, max_lon, 0, 0),
      GeoPoint(max_lat, min_lon, 0, 0),
      GeoPoint(max_lat, max_lon, 0, 0),
  };
  return bbox;
}

std::vector<GeoPoint> PointCollection::bbox(int start, int end) const {
  double min_lat = 90.0;
  double max_lat = -90.0;
  double min_lon = 180.0;
  double max_lon = -180;
  for (auto it = this->begin() + start; it <= this->begin() + end; ++it) {
    min_lat = std::min(it->lat, min_lat);
    max_lat = std::max(it->lat, max_lat);
    min_lon = std::min(it->lon, min_lon);
    max_lon = std::max(it->lon, max_lon);
  }
  std::vector<GeoPoint> bbox = {
      GeoPoint(min_lat, min_lon, 0, 0),
      GeoPoint(min_lat, max_lon, 0, 0),
      GeoPoint(max_lat, min_lon, 0, 0),
      GeoPoint(max_lat, max_lon, 0, 0),
  };
  return bbox;
}

double PointCollection::max_diagonal(int start, int end) const {
  std::vector<GeoPoint> bbox = this->bbox(start, end);
  return std::max(bbox.at(0).distance(bbox.at(3)),
                  bbox.at(1).distance(bbox.at(2)));
}

double PointCollection::max_diagonal() const {
  std::vector<GeoPoint> bbox = this->bbox();
  return std::max(bbox.at(0).distance(bbox.at(3)),
                  bbox.at(1).distance(bbox.at(2)));
}

std::vector<std::pair<double, double>> PointCollection::latlon() const {
  std::vector<std::pair<double, double>> latlon;
  for (const GeoPoint &p : this->geopoints) {
    latlon.push_back(std::make_pair(p.lat, p.lon));
  }
  return latlon;
}

bool PointCollection::set_agl(const std::vector<double> &altitudes) {
  if (altitudes.size() != this->size()) {
    return false;
  }
  for (std::size_t i = 0; i < altitudes.size(); ++i) {
    this->geopoints[i].agl = this->geopoints[i].alt - altitudes[i];
  }
  return true;
}