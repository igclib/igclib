#pragma once

#include <string>

class Time {

public:
  Time(const std::string &str, int offset = 0);
  bool operator<(const Time &t) const;

private:
  int hour = 0;
  int minute = 0;
  int second = 0;
  int sec_from_midnight = 0;
};