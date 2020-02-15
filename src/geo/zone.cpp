#include <boost/algorithm/string.hpp>
#include <boost/geometry.hpp>
#include <igclib/geometry.hpp>
#include <igclib/geopoint.hpp>
#include <igclib/pointcollection.hpp>
#include <igclib/time.hpp>
#include <igclib/util.hpp>
#include <igclib/zone.hpp>
#include <string>
#include <vector>

Zone::Zone(const std::vector<std::string> &openair_record) {
  GeoPoint center_variable;
  bool center_is_set = false;

  std::vector<GeoPoint> polygon_vertices;
  bool clockwise = true;

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
      OpenAirPoint p(r.substr(3));
      polygon_vertices.push_back(p);
    } else if (line_code == "DA") {
      // arc defined by angles
      if (center_is_set) {
        std::vector<std::string> tokens;
        boost::split(tokens, r, [](char c) { return c == ','; });
        int radius = convert::nm2meters(convert::strtoi(tokens[0]));
        double angle_start = convert::strtoi(tokens[1]);
        double angle_end = convert::strtoi(tokens[2]);
        if (!clockwise) {
          double tmp = angle_end;
          angle_end = angle_start;
          angle_start = tmp;
        }
        std::shared_ptr<Geometry> p = std::make_shared<Sector>(
            center_variable, radius, angle_start, angle_end);
        this->geometries.push_back(p);
      }
    } else if (line_code == "DB") {
      // arc defined by coordinates
      // NOT IMPLEMENTED
    } else if (line_code == "DC") {
      // circle
      double radius = convert::nm2meters(std::stod(r.substr(3)));
      if (center_is_set) {
        std::shared_ptr<Geometry> p =
            std::make_shared<Cylinder>(center_variable, radius);
        this->geometries.push_back(p);
        // should center variable be invalidated after adding a circle ?
        // center_is_set = false;
      }
    } else if (line_code == "DY") {
      // airway (how to handle airways?)
      // NOT IMPLEMENTED
    } else if (r.substr(0, 4) == "V D=") {
      // direction assignement for DA and DB records
      if (r.find('-' != std::string::npos)) {
        clockwise = false;
      }
    } else if (r.substr(0, 4) == "V X=") {
      // center assignemnt for DA, DB, and DC records
      center_variable = OpenAirPoint(r.substr(4));
      center_is_set = true;
    }
  }

  // Create polygon with all DP vertices found
  if (!polygon_vertices.empty()) {
    std::shared_ptr<Geometry> p = std::make_shared<Polygon>(polygon_vertices);
    this->geometries.push_back(p);
  }

  // Precompute bounding box for spatial indexing
  mpolygon_t bboxes;
  for (const std::shared_ptr<Geometry> g : this->geometries) {
    polygon_t poly;
    boost::geometry::convert(g->bounding_box(), poly);
    bboxes.push_back(poly);
  }
  boost::geometry::envelope(bboxes, this->bounding_box);
}

std::vector<GeoPoint>
Zone::contained_points(const PointCollection &points) const {
  std::vector<GeoPoint> contained_points;
  for (const std::shared_ptr<Geometry> g : this->geometries) {
    for (const std::pair<Time, GeoPoint> &pair : points) {
      if (this->in_altitude_range(pair.second)) {
        if (g->contains(pair.second)) {
          contained_points.push_back(pair.second);
        }
      }
    }
  }
  return contained_points;
}

bool Zone::in_altitude_range(const GeoPoint &p) const {
  bool higher_than_floor = false;
  bool lower_than_ceiling = false;

  if (((this->floor_is_ground_relative) && (p.agl > this->floor)) ||
      ((!this->floor_is_ground_relative) && (p.alt > this->floor))) {
    higher_than_floor = true;
  }

  if (((this->ceiling_is_ground_relative) && (p.agl < this->ceiling)) ||
      ((!this->floor_is_ground_relative) && (p.alt < this->ceiling))) {
    lower_than_ceiling = true;
  }

  return (higher_than_floor && lower_than_ceiling);
}