#pragma once

#include "igclib/geometry.hpp"
#include "igclib/geopoint.hpp"
#include "igclib/pointcollection.hpp"
#include <ostream>
#include <string>
#include <vector>

typedef boost::geometry::model::box<GeoPoint> box_t;
typedef boost::geometry::model::polygon<GeoPoint> polygon_t;
typedef boost::geometry::model::multi_polygon<polygon_t> mpolygon_t;

class Zone {
  friend class Airspace;

public:
  Zone(const std::vector<std::string> &openair_record);
  std::vector<GeoPoint> contained_points(const PointCollection &points) const;
  bool empty() const { return this->geometries.empty(); };

  friend std::ostream &operator<<(std::ostream &os, const Zone &z);

  box_t bounding_box;

private:
  bool in_altitude_range(const GeoPoint &p) const;

  std::string name;
  std::string cls;

  int ceiling;
  int floor;

  bool ceiling_is_ground_relative;
  bool floor_is_ground_relative;

  // we have to store pointers to geometries to be able to call overloaded
  // virtual methods of derived classes
  std::vector<std::shared_ptr<Geometry>> geometries;
};