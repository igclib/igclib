#pragma once

#include <algorithm>
#include <limits>
#include <string>
#include <vector>

namespace util {

// trim left whitespace in string (in place)
inline void ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                  [](int c) { return !std::isspace(c); }));
}

// trim right whitespace in string (in place)
inline void rtrim(std::string &s) {
  s.erase(
      std::find_if(s.rbegin(), s.rend(), [](int c) { return !std::isspace(c); })
          .base(),
      s.end());
}

// trim whitespace from both side of a string (in place)
inline void trim(std::string &s) {
  ltrim(s);
  rtrim(s);
}

// cartesian product of vectors, nice implementation by
// https://stackoverflow.com/a/17050528/11589613
template <typename T>
inline std::vector<std::vector<T>>
product(const std::vector<std::vector<T>> &v) {
  std::vector<std::vector<T>> s = {{}};
  for (const auto &u : v) {
    std::vector<std::vector<T>> r;
    for (const auto &x : s) {
      for (const auto y : u) {
        r.push_back(x);
        r.back().push_back(y);
      }
    }
    s = std::move(r);
  }
  return s;
}
} // namespace util

namespace convert {
std::pair<bool, int> str2alt(const std::string &s);
int level2meters(const int FL);
int feets2meters(const int f);
int nm2meters(const double nm);
int strtoi(const std::string &s);
} // namespace convert