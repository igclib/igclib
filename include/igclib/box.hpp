#pragma once

#include <array>

#include <igclib/geopoint.hpp>

// boost::geometry::model::box<GeoPoint>

class Box {
public:
  // Box(const GeoPoint &lower_left, const GeoPoint &upper_right);
  // Box(const Flight &flight, std::size_t begin, std::size_t end);
  // double maxdist(const Box &other) const;
  // double mindist(const Box &other) const;

private:
  std::array<GeoPoint, 4> m_corners;
};