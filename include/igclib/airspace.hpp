#pragma once

#include <string>
#include <vector>
#include "igclib/zone.hpp"


class Airspace {
public:
  Airspace(const std::string &airspace_file);

private:
  std::vector<Zone> zones;
};