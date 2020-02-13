#include "igclib/geopoint.hpp"
#include <assert.h>
#include <iostream>

#define PRECISION 0.1

bool almost_equal(double a, double b) { return (abs(a - b) < PRECISION); }

int main() {
  std::string test_cases[] = {
      // BHHMMSSDDMMMMMNDDDMMMMMEVPPPPPCRLF
      "B0000000000000N00000000EA0275001234",
      "B0000000000000N00000000EA0275099999\n",
      "B0000000000000N00000000EA0275004242\r\n",
      "B0000004500000N00000000EA0275004242\r\n",
      "B0000002100000S12000000WA0275000492",
      "B0000004242000N02330000WA0275000264\n",
  };

  GeoPoint expect[] = {
      GeoPoint(0, 0, 1234, 0),     GeoPoint(0, 0, 99999, 0),
      GeoPoint(0, 0, 4242, 0),     GeoPoint(45, 0, 4242, 0),
      GeoPoint(-21, -120, 492, 0), GeoPoint(42.7, -23.5, 264, 0),
  };

  int n_test_cases = sizeof(test_cases) / sizeof(*test_cases);
  int n_expect = sizeof(expect) / sizeof(*expect);
  assert(n_expect == n_test_cases);
  std::vector<int> failures;

  for (int i = 0; i < n_test_cases; i++) {
    IGCPoint p(test_cases[i]);
    if (p.alt != expect[i].alt) {
      failures.push_back(i);
    }
    if (!almost_equal(p.lat, expect[i].lat)) {
      failures.push_back(i);
    }
    if (!almost_equal(p.lon, expect[i].lon)) {
      failures.push_back(i);
    }
  }

  if (!failures.empty()) {
    for (int i : failures){
      std::cerr << "Test " << i << " failed" << std::endl;
    }
  } else {
    std::cerr << "Test passed" << std::endl;
  }
  return EXIT_SUCCESS;
}