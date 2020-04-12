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
  typedef std::map<Zone, geopoints_t> infractions_t;

public:
  Airspace(const std::string &airspace_file);
  std::size_t size() const { return this->zones.size(); };

  void infractions(const PointCollection &points, infractions_t &infractions,
                   bool with_agl) const;

  // needs_agl_checking counts the number of zones in the airspace file which
  // need to be checked against ground altitude. this is to prevent having to
  // ask the elevation service for altitudes if they aren't needed for airspace
  // validation
  std::size_t needs_agl_checking = 0;

private:
  std::vector<Zone> zones;
  bgi::rtree<box_mapping_t, bgi::quadratic<16>> index;
};