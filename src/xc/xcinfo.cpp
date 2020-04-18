#include <igclib/json.hpp>
#include <igclib/pointcollection.hpp>
#include <igclib/xcinfo.hpp>
#include <string>

XCInfo::XCInfo() : m_style(xc_style::UNDEFINED), m_distance(0), m_score(0) {}

XCInfo::XCInfo(xc_style style, double distance, double score)
    : m_style(style), m_distance(distance), m_score(score) {}

bool XCInfo::operator<(const XCInfo &other) const {
  return (this->m_score < other.m_score);
}

bool XCInfo::operator>(const XCInfo &other) const {
  return (this->m_score > other.m_score);
}

// Returns the JSON serialization of an XCInfo
json XCInfo::to_json() const {
  std::string xc_style;
  switch (this->m_style) {
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
          {"distance", this->m_distance / 1000},
          {"score", this->m_score}};
}