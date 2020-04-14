#include <igclib/logging.hpp>
#include <string>

namespace logging {
verbosity default_level = verbosity::WARNING;
void set_level(verbosity level) { default_level = level; }

void error(const std::initializer_list<std::string> &msgs) {
  if (default_level >= verbosity::ERROR) {
    std::cerr << "[\033[31;1m ERROR \033[0m] ";
    for (const std::string &msg : msgs) {
      std::cerr << msg << " ";
    }
    std::cerr << std::endl;
  }
}

void warning(const std::initializer_list<std::string> &msgs) {
  if (default_level >= verbosity::WARNING) {
    std::cerr << "[\033[93;1m WARNING \033[0m] ";

    for (const std::string &msg : msgs) {
      std::cerr << msg << " ";
    }
    std::cerr << std::endl;
  }
}

void debug(const std::initializer_list<std::string> &msgs) {
  if (default_level >= verbosity::DEBUG) {
    std::cerr << "[\033[32;1m DEBUG \033[0m] ";
    for (const std::string &msg : msgs) {
      std::cerr << msg << " ";
    }
    std::cerr << std::endl;
  }
}
} // namespace logging
