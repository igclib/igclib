#include <igclib/pointcollection.hpp>
#include <igclib/xcinfo.hpp>
#include <nlohmann/json.hpp>
#include <string>

XCInfo::XCInfo() {
  this->style = xc_style::UNDEFINED;
  this->distance = 0;
  this->score = 0;
}

XCInfo::XCInfo(xc_style style, double distance, double score) {
  this->style = style;
  this->distance = distance;
  this->score = score;
}

bool XCInfo::operator<(const XCInfo &other) const {
  return (this->score < other.score);
}

bool XCInfo::operator>(const XCInfo &other) const {
  return (this->score > other.score);
}

// Returns the JSON serialization of an XCInfo
nlohmann::json XCInfo::serialize() const {
  std::string xc_style;
  switch (this->style) {
  case xc_style::THREE_POINTS:
    xc_style = "Free distance over three points";
    break;
  case xc_style::FLAT_TRIANGLE:
    xc_style = "Flat triangle";
    break;
  case xc_style::FAI_TRIANGLE:
    xc_style = "FAI Triangle";
    break;
  default:
    xc_style = "Undefined";
    break;
  }
  return {{"style", xc_style},
          {"distance", this->distance / 1000},
          {"score", this->score}};
}