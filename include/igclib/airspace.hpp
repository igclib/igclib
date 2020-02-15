#pragma once

#include <boost/geometry.hpp>
#include <igclib/geopoint.hpp>
#include <igclib/pointcollection.hpp>
#include <igclib/zone.hpp>
#include <string>
#include <vector>

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

typedef boost::geometry::model::box<GeoPoint> box_t;
typedef std::pair<box_t, unsigned int> box_mapping;

class Airspace {
public:
  Airspace(const std::string &airspace_file);
  std::size_t size() const { return this->zones.size(); };

  const std::map<std::string, std::vector<GeoPoint>>
  infractions(const PointCollection &points) const;

private:
  std::vector<Zone> zones;
  bgi::rtree<box_mapping, bgi::quadratic<8>> index;
};