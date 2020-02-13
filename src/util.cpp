#include "igclib/util.hpp"
#include <string>

namespace convert {

int strtoi(const std::string &s) {
  int i = 0;
  for (char c : s) {
    if (c >= '0' && c <= '9') {
      i = i * 10 + (c - '0');
    }
  }
  return i;
}

int feets2meters(const int f) {
  // http://www.sfei.org/it/gis/map-interpretation/conversion-constants
  return int(0.3048 * f);
}

int level2meters(const int FL) { return feets2meters(100 * FL); }

std::pair<bool, int> str2alt(const std::string &s) {
  bool ground_relative = false;
  int altitude;
  if (s.find("SFC") != std::string::npos ||
      s.find("GND") != std::string::npos) {
    altitude = 0;
  } else if (s.find("UNL") != std::string::npos) {
    altitude = std::numeric_limits<int>::max();
  } else if (s.find("FL") != std::string::npos) {
    altitude = level2meters(strtoi(s));
  } else if (s.find("AMSL") != std::string::npos) {
    if (s.find("FT") != std::string::npos) {
      altitude = feets2meters(strtoi(s));
    } else {
      altitude = strtoi(s);
    }
  } else if (s.find("ASFC") != std::string::npos ||
             s.find("AGL") != std::string::npos) {
    ground_relative = true;
    if (s.find("FT") != std::string::npos) {
      altitude = feets2meters(strtoi(s));
    } else {
      altitude = strtoi(s);
    }
  } else {
    // defaults to meters if FT not found !
    altitude = feets2meters(strtoi(s));
  }
  return std::pair<bool, int>(ground_relative, altitude);
}
} // namespace convert