#pragma once

#include <nlohmann/json.hpp>

typedef enum class xc_style {
  UNDEFINED,
  THREE_POINTS,
  FLAT_TRIANGLE,
  FAI_TRIANGLE
} xc_style;

class XCInfo {
public:
  XCInfo();
  XCInfo(xc_style style, double distance, double score);
  nlohmann::json serialize() const;
  bool operator<(const XCInfo &other) const;
  bool operator>(const XCInfo &other) const;

private:
  xc_style style;
  double distance;
  double score;
};