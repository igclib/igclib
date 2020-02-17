#include "igclib/time.hpp"
#include <assert.h>
#include <iostream>
#include <vector>

int main() {
  std::string test_cases[] = {
      // BHHMMSSDDMMMMMNDDDMMMMMEVPPPPPCRLF
      "B0000000000000N00000000EA0275001234",
      "B0842290000000N00000000EA0275099999\n",
      "B2359590000000N00000000EA0275004242\r\n",
      "B1430514500000N00000000EA0275004242\r\n",
      "B1542202100000S12000000WA0275000492",
      "B1024504242000N02330000WA0275000264\n",
  };

  Time expect[] = {Time(0, 0, 0),    Time(8, 42, 29),     Time(23, 59, 59),
                   Time(14, 30, 51), Time(14, 42, 20, 1), Time(15, 24, 50, -5)};

  int n_test_cases = sizeof(test_cases) / sizeof(*test_cases);
  int n_expect = sizeof(expect) / sizeof(*expect);
  assert(n_expect == n_test_cases);
  std::vector<int> failures;

  for (int i = 0; i < n_test_cases; i++) {
    IGCTime p(test_cases[i]);
    if (!((Time)p == expect[i])) {
      failures.push_back(i);
    }
  }

  if (!failures.empty()) {
    for (int i : failures) {
      std::cerr << "Test " << i << " failed" << std::endl;
    }
  } else {
    std::cerr << "Test passed" << std::endl;
  }
  return EXIT_SUCCESS;
}