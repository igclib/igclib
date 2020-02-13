#include "igclib/zone.hpp"
#include "igclib/geometry.hpp"
#include "igclib/geopoint.hpp"
#include "igclib/pointcollection.hpp"
#include "igclib/time.hpp"
#include "igclib/util.hpp"
#include <string>
#include <vector>

Zone::Zone(const std::vector<std::string> &openair_record) {
  GeoPoint center_variable;
  bool center_is_set = false;

  //bool clockwise = true;

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
      this->ceiling_is_ground_relative = alt.first;
      this->ceiling = alt.second;
    } else if (line_code == "AL") {
      // floor
      std::pair<bool, int> alt = convert::str2alt(r.substr(3));
      this->floor_is_ground_relative = alt.first;
      this->floor = alt.second;
    } else if (line_code == "DP") {
      // this->polygon.push_back(r.substr(3));
    } else if (line_code == "DA") {
      // arc defined by angles
    } else if (line_code == "DB") {
      // arc defined by coordinates
    } else if (line_code == "DC") {
      // circle
      double radius = std::stod(r.substr(3));
      if (center_is_set) {
        std::shared_ptr<Geometry> p = std::make_shared<Cylinder>(center_variable, radius);
        this->geometries.push_back(p);
        center_is_set = false;
      }
    } else if (line_code == "DY") {
      // airway
    } else if (r.substr(0, 4) == "V D=") {
      // direction assignment for DA and DB records
    } else if (r.substr(0, 4) == "V X=") {
      
      // center assignemnt for DA, DB, and DC records
    }
  }
}

std::vector<GeoPoint>
Zone::contained_points(const PointCollection &points) const {
  std::vector<GeoPoint> contained_points;
  for (const std::shared_ptr<Geometry> g : this->geometries) {
    for (const std::pair<Time, GeoPoint> &pair : points) {
      if (g->contains(pair.second)) {
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
