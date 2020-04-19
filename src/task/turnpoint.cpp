#include <igclib/geopoint.hpp>
#include <igclib/json.hpp>
#include <igclib/time.hpp>
#include <igclib/turnpoint.hpp>

/** TAKEOFF **/

Takeoff::Takeoff(const GeoPoint &center, const std::size_t &radius,
                 const std::string &name, const std::string &desc)
    : Turnpoint(center, radius, name, desc) {}

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

json Takeoff::to_json() const {
  json j = Turnpoint::to_json();
  j["role"] = "TAKEOFF";
  return j;
}

/** SSS **/

SSS::SSS(const GeoPoint &center, const std::size_t &radius,
         const std::string &name, const std::string &desc, const Time &open,
         bool exit)
    : Turnpoint(center, radius, name, desc), m_open(open), m_exit(exit) {}

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

json SSS::to_json() const {
  json j = Turnpoint::to_json();
  j["open"] = this->m_open.to_string();
  j["role"] = "SSS";
  return j;
}

const Time &SSS::open() const { return this->m_open; }

/** TURNPOINT **/

Turnpoint::Turnpoint(const GeoPoint &center, const std::size_t &radius,
                     const std::string &name, const std::string &desc)
    : m_center(center), m_radius(radius), m_name(name), m_desc(desc) {}

const GeoPoint &Turnpoint::center() const { return this->m_center; }
const std::size_t &Turnpoint::radius() const { return this->m_radius; }

// bool Turnpoint::validate(const GeoPoint &pos) {}

bool Turnpoint::contains(const GeoPoint &pos) {
  return pos.distance(this->m_center) <= this->m_radius;
}

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

json Turnpoint::to_json() const {
  json j;
  j["name"] = this->m_name;
  j["desc"] = this->m_desc;
  j["lat"] = this->m_center.lat;
  j["lon"] = this->m_center.lon;
  j["radius"] = this->m_radius;
  j["alt"] = this->m_center.alt;
  j["role"] = "TURNPOINT";
  return j;
}

/** ESS **/

ESS::ESS(const GeoPoint &center, const std::size_t &radius,
         const std::string &name, const std::string &desc)
    : Turnpoint(center, radius, name, desc) {}

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

json ESS::to_json() const {
  json j = Turnpoint::to_json();
  j["role"] = "ESS";
  return j;
}

/** GOAL **/

Goal::Goal(const GeoPoint &center, const std::size_t &radius,
           const std::string &name, const std::string &desc, const Time &close,
           bool line)
    : Turnpoint(center, radius, name, desc), m_close(close), m_line(line) {}

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

json Goal::to_json() const {
  json j = Turnpoint::to_json();
  j["role"] = "GOAL";
  j["is_line"] = this->is_line();
  return j;
}

bool Goal::contains(const GeoPoint &pos) {
  return Turnpoint::contains(pos) && true; // FIXME different check if line
}

bool Goal::is_line() const { return this->m_line; }

const Time &Goal::close() const { return this->m_close; }