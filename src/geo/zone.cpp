#include "igclib/zone.hpp"
#include "igclib/util.hpp"
#include <string>
#include <vector>

Zone::Zone(const std::vector<std::string> &record) {
  for (std::string r : record) {
    util::trim(r);
    if (r.substr(0, 2) == "AC") {
      this->cls = r.substr(2);
    } else if (r.substr(0, 2) == "AN") {
      this->name = r.substr(2);
    } else if (r.substr(0, 2) == "AH") {
      auto a = r.substr(2);
      std::pair<bool, int> alt = convert::str2alt(a);
      this->ceiling_ground_relative = alt.first;
      this->ceiling = alt.second;
    } else if (r.substr(0, 2) == "AL") {
      auto a = r.substr(2);
      std::pair<bool, int> alt = convert::str2alt(a);
      this->floor_ground_relative = alt.first;
      this->floor = alt.second;
    }
  }
}