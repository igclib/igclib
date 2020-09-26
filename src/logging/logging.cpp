#include <igclib/logging.hpp>
#include <string>

namespace logging {
verbosity default_level = verbosity::WARNING;
bool pretty_print = false;
void set_level(verbosity level) { default_level = level; }
void set_pretty_print(bool enabled) { pretty_print = enabled; }

void error(const std::initializer_list<std::string> &msgs) {
  if (default_level >= verbosity::ERROR) {
    if (pretty_print) {
      std::cerr << "[\033[31;1m ERROR \033[0m] ";
    }
    for (const std::string &msg : msgs) {
      std::cerr << msg << " ";
    }
    std::cerr << std::endl;
  }
}

void warning(const std::initializer_list<std::string> &msgs) {
  if (default_level >= verbosity::WARNING) {
    if (pretty_print) {
      std::cerr << "[\033[93;1m WARNING \033[0m] ";
    }
    for (const std::string &msg : msgs) {
      std::cerr << msg << " ";
    }
    std::cerr << std::endl;
  }
}

void debug(const std::initializer_list<std::string> &msgs) {
  if (default_level >= verbosity::DEBUG) {
    if (pretty_print) {
      std::cerr << "[\033[32;1m DEBUG \033[0m] ";
    }
    for (const std::string &msg : msgs) {
      std::cerr << msg << " ";
    }
    std::cerr << std::endl;
  }
}

void info(const std::initializer_list<std::string> &msgs) {
  if (default_level > verbosity::QUIET) {
    if (pretty_print) {
      std::cerr << "[\033[32;1m INFO \033[0m] ";
    }
    for (const std::string &msg : msgs) {
      std::cerr << msg << " ";
    }
    std::cerr << std::endl;
  }
}

} // namespace logging
