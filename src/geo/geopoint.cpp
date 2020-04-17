#include <GeographicLib/Geodesic.hpp>
#include <igclib/geopoint.hpp>
#include <igclib/json.hpp>
#include <iostream>
#include <stdexcept>

bool GeoPoint::operator==(const GeoPoint &other) const {
  return (this->id() == other.id());
}

std::string GeoPoint::id() const { return this->m_id; }

IGCPoint::IGCPoint(const std::string &str) {
  // http://vali.fai-civl.org/documents/IGC-Spec_v1.00.pdf
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
  this->agl = -1;

  if ((this->lon < -180.0) || (lon > 180.0)) {
    throw std::runtime_error("Longitude must be between -180 and 180");
  }

  if ((this->lat < -90.0) || (this->lat > 90.0)) {
    throw std::runtime_error(
        "Absolute value of latitude must be between 0 and 90");
  }
}

OpenAirPoint::OpenAirPoint(const std::string &str) {
  // http://www.winpilot.com/UsersGuide/UserAirspace.asp
  // spec seems really flexible
  double deg;
  double min;
  double sec;
  double lat;
  double lon;
  std::string tokens[4];
  std::vector<std::string> dms;

  size_t lat_delim = str.find_first_of("NS");
  size_t lon_delim = str.find_first_of("EW");
  tokens[0] = str.substr(0, lat_delim);
  tokens[1] = str.substr(lat_delim, 1);
  tokens[2] = str.substr(lat_delim + 1, lon_delim - (lat_delim + 1));
  tokens[3] = str.substr(lon_delim, 1);

  boost::split(dms, tokens[0],
               [](char c) { return (!std::isdigit(c) && !std::isspace(c)); });
  if (dms.size() != 3) {
    throw std::runtime_error("Invalid OpenAir record" + str);
  }
  deg = std::stod(dms[0]);
  min = std::stod(dms[1]);
  sec = std::stod(dms[2]);
  lat = deg + min / 60 + sec / 3600;
  this->lat = tokens[1] == "N" ? lat : -lat;

  boost::split(dms, tokens[2],
               [](char c) { return (!std::isdigit(c) && !std::isspace(c)); });
  if (dms.size() != 3) {
    throw std::runtime_error("Invalid OpenAir record" + str);
  }
  deg = std::stod(dms[0]);
  min = std::stod(dms[1]);
  sec = std::stod(dms[2]);
  lon = deg + min / 60 + sec / 3600;
  this->lon = tokens[3] == "E" ? lon : -lon;

  this->alt = -1;
  this->agl = -1;
}

GeoPoint::GeoPoint(double lat, double lon, int alt, int agl) {
  this->lat = lat;
  this->lon = lon;
  this->alt = alt;
  this->agl = agl;
  this->m_id = std::to_string(this->lat) + ":" + std::to_string(this->lon);
}

GeoPoint::GeoPoint(double lat, double lon, int alt) {
  this->lat = lat;
  this->lon = lon;
  this->alt = alt;
  this->agl = -1;
  this->m_id = std::to_string(this->lat) + ":" + std::to_string(this->lon);
}

double GeoPoint::distance(const GeoPoint &p) const {
  GeographicLib::Geodesic geod = GeographicLib::Geodesic::WGS84();
  double distance;
  geod.Inverse(this->lat, this->lon, p.lat, p.lon, distance);
  return distance;
}

double GeoPoint::heading(const GeoPoint &p) const {
  GeographicLib::Geodesic geod = GeographicLib::Geodesic::WGS84();
  double heading;
  double unused;
  geod.Inverse(this->lat, this->lon, p.lat, p.lon, heading, unused);
  return heading;
}

GeoPoint GeoPoint::project(double heading, double distance) const {
  GeographicLib::Geodesic geod = GeographicLib::Geodesic::WGS84();
  double lat;
  double lon;
  geod.Direct(this->lat, this->lon, heading, distance, lat, lon);
  return GeoPoint(lat, lon, -1, -1);
}

json GeoPoint::to_json() const {
  json j = {
      {"lat", this->lat},
      {"lon", this->lon},
  };
  if (this->alt != -1) {
    j["alt"] = this->alt;
  }
  if (this->agl != -1) {
    j["agl"] = this->agl;
  }
  return j;
}