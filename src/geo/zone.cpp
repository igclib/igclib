#include "igclib/zone.hpp"
#include "igclib/geopoint.hpp"
#include "igclib/pointcollection.hpp"
#include "igclib/time.hpp"
#include "igclib/util.hpp"
#include <string>
#include <vector>

Zone::Zone(const std::vector<std::string> &openair_record) {
  for (std::string r : openair_record) {
    std::string line_code = r.substr(0, 2);
    if (line_code == "AC") {
      // class
      this->cls = r.substr(3);
    } else if (line_code == "AN") {
      // name
      this->name = r.substr(3);
    } else if (line_code == "AH") {
      // ceiling
      std::pair<bool, int> alt = convert::str2alt(r.substr(3));
      this->ceiling_ground_relative = alt.first;
      this->ceiling = alt.second;
    } else if (line_code == "AL") {
      // floor
      std::pair<bool, int> alt = convert::str2alt(r.substr(3));
      this->floor_ground_relative = alt.first;
      this->floor = alt.second;
    } else if (line_code == "DP") {
      // this->polygon.push_back(r.substr(3));
    } else if (line_code == "DA") {
      // arc defined by angles
    } else if (line_code == "DB") {
      // arc defined by coordinates
    } else if (line_code == "DC") {
      // circle
    } else if (line_code == "DY") {
      // airway
    } else if (r.substr(0, 4) == "V X=") {
      // variable assignement
    }
  }
}

std::vector<GeoPoint>
Zone::contained_points(const PointCollection &points) const {
  std::vector<GeoPoint> contained_points;
  for (const Geometry &g : this->geometries) {
    for (const std::pair<Time, GeoPoint> &pair : points) {
      if (g.contains(pair.second)) {
        contained_points.push_back(pair.second);
      }
    }
  }
  return contained_points;
}

std::ostream &operator<<(std::ostream &os, const Zone &z) {
  os << z.name;
  return os;
}
