#include "igclib/time.hpp"
#include "stdlib.h"
#include <stdexcept>
#include <string>

Time::Time(const std::string &str, int offset /* = 0 */) {
  if (str.length() != 6) {
    throw std::runtime_error("String must have a length of exactly 6");
  }

  this->hour = std::stoi(str.substr(0, 2)) + offset;
  this->minute = std::stoi(str.substr(2, 2));
  this->second = std::stoi(str.substr(4, 2));

  if ((this->hour < 0) || (this->hour > 23)) {
    throw std::runtime_error("Hour must be between 0 and 23");
  }
  if ((this->minute < 0) || (this->minute > 59)) {
    throw std::runtime_error("Minute must be between 0 and 59");
  }
  if ((this->second < 0) || (this->second > 59)) {
    throw std::runtime_error("Second must be between 0 and 59");
  }
  
  this->sec_from_midnight = (3600 * this->hour) + (60 * this->minute) + this->second;
}

bool Time::operator<(const Time &t) const {
  return this->sec_from_midnight < t.sec_from_midnight;
}

bool Time::operator==(const Time &t) const {
  return this->sec_from_midnight == t.sec_from_midnight;
}