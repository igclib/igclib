#include <igclib/geopoint.hpp>
#include <igclib/time.hpp>
#include <igclib/turnpoint.hpp>

Turnpoint::Turnpoint(const GeoPoint &center, const std::size_t &radius,
                     const std::string &name, const std::string &desc)
    : m_center(center), m_radius(radius), m_name(name), m_desc(desc) {}

std::string Turnpoint::to_string() const {
  std::string res;
  res += "TP ";
  res += this->m_name;
  res += " ";
  res += this->m_desc;
  res += " - ";
  res += std::to_string(this->m_radius) + "m";
  return res;
}

std::string Takeoff::to_string() const {
  std::string res;
  res += "TAKEOFF ";
  res += this->m_name;
  res += " ";
  res += this->m_desc;
  res += " - ";
  res += std::to_string(this->m_radius) + "m";
  return res;
}

std::string SSS::to_string() const {
  std::string res;
  res += "SSS ";
  res += this->m_name;
  res += " ";
  res += this->m_desc;
  res += " - ";
  res += std::to_string(this->m_radius) + "m ";
  res += "- opens at " + this->m_open.to_string();
  return res;
}

std::string ESS::to_string() const {
  std::string res;
  res += "ESS ";
  res += this->m_name;
  res += " ";
  res += this->m_desc;
  res += " - ";
  res += std::to_string(this->m_radius) + "m";
  return res;
}

std::string Goal::to_string() const {
  std::string res;
  res += "GOAL ";
  if (this->is_line()) {
    res += "LINE ";
  } else {
    res += "CYLINDER ";
  }
  res += this->m_name;
  res += " ";
  res += this->m_desc;
  res += " - ";
  res += std::to_string(this->m_radius) + "m ";
  res += "- closes at " + this->m_close.to_string();
  return res;
}

Takeoff::Takeoff(const GeoPoint &center, const std::size_t &radius,
                 const std::string &name, const std::string &desc)
    : Turnpoint(center, radius, name, desc) {}

ESS::ESS(const GeoPoint &center, const std::size_t &radius,
         const std::string &name, const std::string &desc)
    : Turnpoint(center, radius, name, desc) {}

SSS::SSS(const GeoPoint &center, const std::size_t &radius,
         const std::string &name, const std::string &desc, const Time &open)
    : Turnpoint(center, radius, name, desc), m_open(open) {}

Goal::Goal(const GeoPoint &center, const std::size_t &radius,
           const std::string &name, const std::string &desc, const Time &close,
           bool line)
    : Turnpoint(center, radius, name, desc), m_close(close), m_line(line) {}

bool Goal::is_line() const { return this->m_line; }