#pragma once

#include <string>

class GeoPoint {
public:
  GeoPoint(const std::string &str);

  double lat;
  double lon;
  double alt;
};