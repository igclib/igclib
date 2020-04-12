#pragma once

#include <igclib/flight.hpp>
#include <igclib/geopoint.hpp>

class Box {
public:
  Box();
  Box(const GeoPoint &lower_left, const GeoPoint &upper_right);
  Box(const Flight &flight, std::size_t begin, std::size_t end);
  double maxdist(const Box &other) const;
  double mindist(const Box &other) const;

private:
};