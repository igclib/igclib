#include "igclib/geo.hpp"
#include <stdexcept>

GeoPoint::GeoPoint(const std::string &str) {
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

  //decode altitude
  //double baro_alt = std::stoi(str.substr(18, 5));
  double gps_alt = std::stoi(str.substr(23, 5));
  this->alt = gps_alt;
}

GeoPoint::GeoPoint(const GeoPoint& g){
  this->lat = g.lat;
  this->lon = g.lon;
  this->alt = g.alt;
}

GeoPoint::GeoPoint(){
  this->lat = 0;
  this->lon = 0;
  this->alt = -1;
}