#pragma once

#include <algorithm>
#include <limits>
#include <string>

namespace util {

// trim left whitespace in string (in place)
inline void ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                  [](int ch) { return !std::isspace(ch); }));
}

// trim right whitespace in string (in place)
inline void rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(),
                       [](int ch) { return !std::isspace(ch); })
              .base(),
          s.end());
}

// trim whitespace from both side of a string (in place)
inline void trim(std::string &s) {
  ltrim(s);
  rtrim(s);
}

} // namespace util

namespace convert {
std::pair<bool, int> str2alt(const std::string &s);
int level2meters(const int FL);
int feets2meters(const int f);
} // namespace convert