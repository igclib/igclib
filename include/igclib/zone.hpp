#pragma once

#include <string>
#include <vector>

class Zone {
public:
  Zone(const std::vector<std::string> &record);

private:
  std::string name;
  std::string cls;
  std::string comment;
  int ceiling;
  int floor;
  bool ceiling_ground_relative;
  bool floor_ground_relative;
  // std::vector<Geometry> parts;
};