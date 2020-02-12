#pragma once

#include <string>

class Time {

public:
  Time(const std::string &str, int offset = 0);
  size_t hash(void) const { return this->sec_from_midnight; };
  bool operator<(const Time &t) const;
  bool operator==(const Time &t) const;

private:
  int hour = 0;
  int minute = 0;
  int second = 0;
  int sec_from_midnight = 0;
};

namespace std {
// specialize hash function to insert Time in unordered map
template <> struct hash<Time> {
  std::size_t operator()(const Time &t) const { return t.hash(); }
};

} // namespace std