#include "igclib/geopoint.hpp"
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <stdexcept>

IGCPoint::IGCPoint(const std::string &str) {
  double DD;
  double dd;
  double lat;
  double lon;
  std::string tokens[] = {str.substr(7, 2),  str.substr(9, 5),
                          str.substr(14, 1), str.substr(15, 3),
                          str.substr(18, 5), str.substr(23, 1),
                          str.substr(25, 5), str.substr(30, 5)};

  
  // decode latitude
  DD = std::stod(tokens[0]);
  dd = std::stod(tokens[1]) / 1000.0;
  lat = DD + dd / 60.0;
  this->lat = tokens[2] == "N" ? lat : -lat;

  // decode longitude
  DD = std::stod(tokens[3]);
  dd = std::stod(tokens[4]) / 1000.0;
  lon = DD + dd / 60.0;
  this->lon = tokens[5] == "E" ? lon : -lon;

  // decode altitude
  this->alt = std::stoi(tokens[7]);
  // all ground altitudes should be set in a batch request to the elevation API
  this->ground_alt = 0;

  if ((this->lon < -180.0) || (lon > 180.0)) {
    throw std::runtime_error("Longitude must be between -180 and 180");
  }

  if ((this->lat < -90.0) || (this->lat > 90.0)) {
    throw std::runtime_error(
        "Absolute value of latitude must be between 0 and 90");
  }
}

OpenAirPoint::OpenAirPoint(const std::string &str) {
  double deg;
  double min;
  double sec;
  double lat;
  double lon;
  std::vector<std::string> tokens;
  std::vector<std::string> dms;

  boost::split(tokens, str, [](char c) { return std::isspace(c); });
  if (tokens.size() < 4) {
    throw std::runtime_error("Invalid OpenAir record" + str);
  }

  boost::split(dms, tokens[0], [](char c) { return !std::isdigit(c); });
  if (dms.size() != 3) {
    throw std::runtime_error("Invalid OpenAir record" + str);
  }
  deg = std::stod(dms[0]);
  min = std::stod(dms[1]);
  sec = std::stod(dms[2]);
  lat = deg + min / 60 + sec / 3600;
  this->lat = tokens[1] == "N" ? lat : -lat;

  boost::split(dms, tokens[2], [](char c) { return !std::isdigit(c); });
  if (dms.size() != 3) {
    throw std::runtime_error("Invalid OpenAir record" + str);
  }
  deg = std::stod(dms[0]);
  min = std::stod(dms[1]);
  sec = std::stod(dms[2]);
  lon = deg + min / 60 + sec / 3600;
  this->lon = tokens[3] == "E" ? lon : -lon;

  this->alt = 0;
  this->ground_alt = 0;
}

GeoPoint::GeoPoint(double lat, double lon, int alt, int ground_alt){
  this->lat = lat;
  this->lon = lon;
  this->alt = alt;
  this->ground_alt = ground_alt;
}