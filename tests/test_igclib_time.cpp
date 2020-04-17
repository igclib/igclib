#include <igclib/logging.hpp>
#include <igclib/time.hpp>
#include <iostream>

int main() {
  Time t1;
  Time t2 = t1;

  // increment/decrement -> same value
  ++t1;
  --t1;
  if (t1 != t2) {
    logging::error({"[inc/dec] test failed"});
  }

  // underflow error
  try {
    --t1;
  } catch (const std::runtime_error &e) {
    if (std::string(e.what()) != "Time underflow")
      logging::error({"[underflow] test failed"});
  }

  // overflow error
  t2 = Time(23, 59, 59);
  try {
    ++t2;
  } catch (const std::runtime_error &e) {
    if (std::string(e.what()) != "Time overflow")
      logging::error({"[overflow] test failed"});
  }

  // offset
  t1 = Time(1, 30, 42);
  t2 = Time(8, 51, 50, t1);
  Time t3(10, 22, 32);
  if (t2 != t3) {
    logging::error({"[offset] test failed"});
  }

  // addition
  t2 = Time(8, 51, 50);
  if (t1 + t2 != t3) {
    logging::error({"[addition] test failed"});
  }

  // subtraction
  t3 = t2 - t1;
  if (t3.to_string() != "07:21:08") {
    logging::error({"[subtraction] test failed"});
  }

  logging::info({"test passed"});
  return EXIT_SUCCESS;
}