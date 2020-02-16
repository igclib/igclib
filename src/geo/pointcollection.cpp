#include <igclib/pointcollection.hpp>
#include <iostream>
#include <iterator>
#include <vector>

PointCollection::PointCollection() {
  geopoints = {};
  timepoints = {};
}

PointCollection ::PointCollection(geopoints_t::const_iterator start,
                                  geopoints_t::const_iterator end) {
  geopoints = geopoints_t(start, end);
}

void PointCollection::insert(const Time &t, const GeoPoint &p) {
  // is this really safe ? do some testing
  timepoints.emplace(t, geopoints.size());
  geopoints.push_back(p);
}

const GeoPoint &PointCollection::operator[](const Time &t) const {
  size_t index = timepoints.at(t);
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

double PointCollection::bbox_area() const {
  std::vector<GeoPoint> bbox = this->bbox();
  return bbox[0].distance(bbox[3]);
}

std::pair<PointCollection, PointCollection> PointCollection::split() const {
  size_t half_size = this->size() / 2;
  PointCollection p1(this->begin(), this->begin() + half_size);
  PointCollection p2(this->begin() + half_size, this->end());
  return std::make_pair(p1, p2);
}