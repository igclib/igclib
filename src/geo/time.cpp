#include "igclib/time.hpp"
#include <stdexcept>
#include <string>

IGCTime::IGCTime(const std::string &str, int offset /* = 0 */) {
  this->hour = std::stoi(str.substr(1, 2)) + offset;
  this->minute = std::stoi(str.substr(3, 2));
  this->second = std::stoi(str.substr(5, 2));
  this->sec_from_midnight =
      3600 * this->hour + 60 * this->minute + this->second;

  if ((this->hour < 0) || (this->hour > 23)) {
    throw std::runtime_error("Hour must be between 0 and 23");
  }
  if ((this->minute < 0) || (this->minute > 59)) {
    throw std::runtime_error("Minute must be between 0 and 59");
  }
  if ((this->second < 0) || (this->second > 59)) {
    throw std::runtime_error("Second must be between 0 and 59");
  }
}

bool Time::operator==(const Time &t) const {
  return this->sec_from_midnight == t.sec_from_midnight;
}

Time::Time(int hour, int minute, int second, int offset /* = 0*/) {
  this->hour = hour + offset;
  this->minute = minute;
  this->second = second;
  this->sec_from_midnight =
      3600 * this->hour + 60 * this->minute + this->second;
}
