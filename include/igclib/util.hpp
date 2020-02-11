#include <algorithm>

namespace util {
// trim left whitespace in string (in place)
static inline void ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                  [](int ch) { return !std::isspace(ch); }));
}

// trim right whitespace in string (in place)
static inline void rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(),
                       [](int ch) { return !std::isspace(ch); })
              .base(),
          s.end());
}

// trim whitespace from both side of a string (in place)
static inline void trim(std::string &s) {
  ltrim(s);
  rtrim(s);
}

} // namespace util