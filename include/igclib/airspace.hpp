#pragma once

#include <boost/geometry.hpp>
#include <igclib/geopoint.hpp>
#include <igclib/pointcollection.hpp>
#include <igclib/zone.hpp>
#include <string>
#include <vector>

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

class Airspace {
  typedef bg::model::linestring<GeoPoint> linestring_t;
  typedef bg::model::box<GeoPoint> box_t;
  typedef std::pair<bg::model::box<GeoPoint>, size_t> box_mapping_t;
  typedef std::vector<GeoPoint> geopoints_t;
  typedef std::map<std::string, geopoints_t> infractions_t;

public:
  Airspace(const std::string &airspace_file);
  size_t size() const { return zones.size(); };

  void infractions(const PointCollection &points,
                   infractions_t &infractions) const;

private:
  std::vector<Zone> zones;
  bgi::rtree<box_mapping_t, bgi::quadratic<8>> index;
};