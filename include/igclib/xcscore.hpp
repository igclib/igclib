#pragma once

#include <nlohmann/json.hpp>

enum class xc_style { UNDEFINED, THREE_POINTS, FLAT_TRIANGLE, FAI_TRIANGLE };

class XCScore {

public:
  XCScore();
  XCScore(xc_style &style, double distance, double score);
  nlohmann::json serialize() const;

  xc_style style;
  double distance;
  double score;
};