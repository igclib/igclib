#pragma once

#include <igclib/geopoint.hpp>
#include <unordered_map>

namespace cache {
void print_stats();
double distance(const GeoPoint &p1, const GeoPoint &p2);
} // namespace cache