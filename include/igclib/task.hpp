#pragma once

#include <string>

class Task {
public:
protected:
};

class FFVLTask : public Task {};

class XCTask : public Task {
  XCTask(std::string fp);
};
