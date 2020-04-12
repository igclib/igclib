#pragma once

#include "igclib/geometry.hpp"
#include "igclib/geopoint.hpp"
#include "igclib/pointcollection.hpp"
#include <nlohmann/json.hpp>
#include <ostream>
#include <string>
#include <vector>

class Zone {
  typedef boost::geometry::model::box<GeoPoint> box_t;
  typedef boost::geometry::model::polygon<GeoPoint> polygon_t;
  typedef boost::geometry::model::multi_polygon<polygon_t> mpolygon_t;
  friend class Airspace;

public:
  Zone(const std::vector<std::string> &openair_record);
  std::vector<GeoPoint> contained_points(const PointCollection &points,
                                         bool with_agl) const;
  // TODO add correct method to check if minimum attributes are set and
  // prevent uninitialized conditional jumps
  bool empty() const { return geometries.empty(); };
  bool needs_agl_checking() const;
  bool operator<(const Zone &other) const;
  std::string name() const { return this->m_name; };
  const nlohmann::json to_json() const;

private:
  bool in_altitude_range(const GeoPoint &p, bool with_agl) const;

  std::string m_name;
  std::string cls;

  std::string floor_txt;
  std::string ceiling_txt;

  int ceiling;
  int floor;

  bool ceiling_is_ground_relative;
  bool floor_is_ground_relative;

  // necessary to store pointers to geometries to be able to call overloaded
  // virtual methods of derived classes
  std::vector<std::shared_ptr<Geometry>> geometries;
  box_t bounding_box;
};