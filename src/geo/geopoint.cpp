#include "igclib/geopoint.hpp"
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <stdexcept>

IGCPoint::IGCPoint(const std::string &str) {
  // decode latitude
  double d = std::stod(str.substr(0, 2));
  double m = std::stod(str.substr(2, 6)) / 1000.0;
  char hemisphere = str[7];
  double l = d + m / 60.0;

  if ((l < 0.0) || (l > 90.0)) {
    throw std::runtime_error(
        "Absolute value of latitude must be between 0 and 90");
  }
  this->lat = (hemisphere == 'N') ? l : -l;

  // decode longitude
  d = std::stod(str.substr(8, 2));
  m = std::stod(str.substr(10, 6)) / 1000.0;
  hemisphere = str[16];
  l = d + m / 60.0;
  if ((l < 0.0) || (l > 180.0)) {
    throw std::runtime_error("Longitude must be between 0 and 180");
  }
  this->lon = (hemisphere == 'E') ? l : -l;

  // decode altitude
  // double baro_alt = std::stoi(str.substr(18, 5));
  double gps_alt = std::stoi(str.substr(23, 5));
  this->alt = gps_alt;
  // all ground altitudes should be set in a batch request to the elevation API
  this->ground_alt = 0;
}

OpenAirPoint::OpenAirPoint(const std::string &str) {
  bool nothern_hemisphere;
  bool eastern_hemisphere;
  double d;
  double m;
  double s;
  double l;
  std::vector<std::string> tokens;
  std::vector<std::string> dms;

  boost::split(tokens, str, [](char c) { return std::isspace(c); });
  if (tokens.size() < 4) {
    throw std::runtime_error("Invalid OpenAir record" + str);
  }
  if (tokens[1] == "N") {
    nothern_hemisphere = true;
  } else if (tokens[1] == "S") {
    nothern_hemisphere = false;
  } else {
    throw std::runtime_error("Invalid OpenAir record" + str);
  }
  if (tokens[3] == "E") {
    eastern_hemisphere = true;
  } else if (tokens[3] == "W") {
    eastern_hemisphere = false;
  } else {
    throw std::runtime_error("Invalid OpenAir record" + str);
  }

  boost::split(dms, tokens[0], [](char c) { return !std::isdigit(c); });
  if (dms.size() != 3) {
    throw std::runtime_error("Invalid OpenAir record" + str);
  }
  d = std::stod(dms[0]);
  m = std::stod(dms[1]);
  s = std::stod(dms[2]);
  l = d + m / 60 + s / 3600;
  this->lat = nothern_hemisphere ? l : -l;

  boost::split(dms, tokens[2], [](char c) { return !std::isdigit(c); });
  if (dms.size() != 3) {
    throw std::runtime_error("Invalid OpenAir record" + str);
  }
  d = std::stod(dms[0]);
  m = std::stod(dms[1]);
  s = std::stod(dms[2]);
  l = d + m / 60 + s / 3600;
  this->lon = eastern_hemisphere ? l : -l;

  this->alt = 0;
  this->ground_alt = 0;
}