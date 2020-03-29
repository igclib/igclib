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

template <typename T>
std::vector<std::vector<T>> product(const std::vector<std::vector<T>> &lists) {
  {
    std::vector<std::vector<T>> result;
    if (std::find_if(std::begin(lists), std::end(lists), [](auto e) -> bool {
          return e.size() == 0;
        }) != std::end(lists)) {
      return result;
    }
    for (auto &e : lists[0]) {
      result.push_back({e});
    }
    for (size_t i = 1; i < lists.size(); ++i) {
      std::vector<std::vector<T>> temp;
      for (auto &e : result) {
        for (auto f : lists[i]) {
          auto e_tmp = e;
          e_tmp.push_back(f);
          temp.push_back(e_tmp);
        }
      }
      result = temp;
    }
    return result;
  }
}

} // namespace util

namespace convert {
std::pair<bool, int> str2alt(const std::string &s);
int level2meters(const int FL);
int feets2meters(const int f);
int nm2meters(const double nm);
int strtoi(const std::string &s);
} // namespace convert