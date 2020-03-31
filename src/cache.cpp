#include <igclib/cache.hpp>
#include <igclib/geopoint.hpp>
#include <iostream>

namespace cache {

static std::unordered_map<std::string, double> dist_cache;
static int cache_hit;
static int cache_miss;

void print_stats() {
  std::cout << "Cache hit : " << cache_hit << " Cache miss : " << cache_miss
            << std::endl;
}

double distance(const GeoPoint &p1, const GeoPoint &p2) {
  if (dist_cache.empty()) {
    dist_cache.reserve(20000);
  }
  auto id = p1.id() + p2.id();
  auto found = dist_cache.find(id);
  if (found != dist_cache.end()) {
    cache_hit++;
    return found->second;
  } else {
    cache_miss++;
    double distance = p1.distance(p2);
    auto reversed_id = p2.id() + p1.id();
    dist_cache[id] = distance;
    dist_cache[reversed_id] = distance;
    return distance;
  }
}
} // namespace cache