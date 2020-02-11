#pragma once

#include <string>

class Flight {

public:
  Flight(std::string filename);
  void json();

private:
  std::string pilot_name;
};