#pragma once

#include <boost/geometry.hpp>
#include <igclib/box.hpp>
#include <igclib/geopoint.hpp>
#include <igclib/pointcollection.hpp>
#include <igclib/zone.hpp>
#include <memory>
#include <string>
#include <vector>

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

class Airspace {
  typedef std::pair<bg::model::box<GeoPoint>, std::shared_ptr<Zone>>
      box_mapping_t;
  typedef std::vector<std::pair<std::shared_ptr<Zone>, PointCollection>>
      infractions_t;

public:
  Airspace(const std::string &airspace_file);
  std::size_t size() const { return this->index.size(); };

  void infractions(const PointCollection &flight, infractions_t &infractions,
                   bool with_agl) const;

  // needs_agl_checking counts the number of zones in the airspace file which
  // need to be checked against ground altitude. this is to prevent having to
  // ask the elevation service for altitudes if they aren't needed for airspace
  // validation
  std::size_t needs_agl_checking = 0;

private:
  void update_index(std::vector<std::string> &record);
  bgi::rtree<box_mapping_t, bgi::quadratic<16>> index;
};