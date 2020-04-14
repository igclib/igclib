#pragma once

#include <memory>
#include <string>

typedef enum TaskFormat { XCTRACK, PWCA, FFVL, UNKOWN } TaskFormat;

class TaskImpl {};

class FFVLTask : public TaskImpl {};

class XCTask : public TaskImpl {
  XCTask(std::string fp);
};

class Task {
public:
  Task();
  Task(const std::string &task_file);
  void identify(const std::string &task_file, TaskFormat &tf) const;
  void save(const std::string &out) const;

protected:
  std::shared_ptr<TaskImpl> task;
  TaskFormat format = TaskFormat::UNKOWN;
};
