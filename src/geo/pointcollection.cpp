#include <igclib/pointcollection.hpp>
#include <iostream>
#include <iterator>
#include <vector>

void PointCollection::insert(const Time &t, const GeoPoint &p) {
  auto &&pt = std::make_shared<GeoPoint>(p);
  this->m_timepoints.emplace_hint(this->m_timepoints.end(), t, pt);
  this->m_geopoints.push_back(pt);
}

void PointCollection::insert(
    const std::pair<const Time, std::shared_ptr<GeoPoint>> &p) {
  this->m_timepoints.emplace_hint(this->m_timepoints.end(), p.first, p.second);
  this->m_geopoints.push_back(p.second);
}

const std::shared_ptr<GeoPoint> PointCollection::at(std::size_t index) const {
  return this->m_geopoints.at(index);
}

const std::shared_ptr<GeoPoint> PointCollection::at(const Time &t) const {
  return this->m_timepoints.at(t);
}

const std::array<GeoPoint, 4> PointCollection::bbox() const {
  return this->bbox(0, this->size());
}

const std::array<GeoPoint, 4> PointCollection::bbox(std::size_t start,
                                                    std::size_t end) const {
  if (end > this->size()) {
    throw std::runtime_error("Accessing point outside of collection");
  }

  double min_lat = 90.0;
  double max_lat = -90.0;
  double min_lon = 180.0;
  double max_lon = -180;

  for (auto it = this->geopoints().begin() + start;
       it <= this->geopoints().begin() + end; ++it) {
    min_lat = std::min(it->get()->lat, min_lat);
    max_lat = std::max(it->get()->lat, max_lat);
    min_lon = std::min(it->get()->lon, min_lon);
    max_lon = std::max(it->get()->lon, max_lon);
  }

  std::array<GeoPoint, 4> &&box = {
      GeoPoint(min_lat, min_lon, 0, 0),
      GeoPoint(min_lat, max_lon, 0, 0),
      GeoPoint(max_lat, min_lon, 0, 0),
      GeoPoint(max_lat, max_lon, 0, 0),
  };
  return box;
}

double PointCollection::max_diagonal(int start, int end) const {
  std::array<GeoPoint, 4> bbox = this->bbox(start, end);
  return std::max(bbox.at(0).distance(bbox.at(3)),
                  bbox.at(1).distance(bbox.at(2)));
}

double PointCollection::max_diagonal() const {
  std::array<GeoPoint, 4> bbox = this->bbox();
  return std::max(bbox.at(0).distance(bbox.at(3)),
                  bbox.at(1).distance(bbox.at(2)));
}

const std::vector<std::pair<double, double>> PointCollection::latlon() const {
  std::vector<std::pair<double, double>> latlon;
  for (const auto &p : this->m_geopoints) {
    latlon.push_back(std::make_pair(p->lat, p->lon));
  }
  return latlon;
}

bool PointCollection::set_agl(const std::vector<double> &altitudes) {
  if (altitudes.size() != this->size()) {
    return false;
  }
  for (std::size_t i = 0; i < altitudes.size(); ++i) {
    this->m_geopoints.at(i)->agl =
        this->m_geopoints.at(i)->alt - altitudes.at(i);
  }
  return true;
}

json PointCollection::to_json() const {
  json j;
  for (const auto &timepoint : this->timepoints()) {
    j[timepoint.first.to_string()] = timepoint.second->to_json();
  }
  return j;
}