#ifndef UDEN_EVENT
#define UDEN_EVENT

#include <variant>
#include <queue>
#include <iostream>

struct enableSketchMode {};
struct disableSketchMode {};
struct toggleSketchMode {};
struct popMode {};
struct togglePointMode {};
struct startPan {};
struct stopPan {};

struct exitProgram {};

using AppEvent = std::variant<
  enableSketchMode,
  disableSketchMode,
  toggleSketchMode,
  popMode,
  startPan,
  stopPan,
  exitProgram
>;

class EventQueue {
public:
  EventQueue();
  ~EventQueue();

  static EventQueue* getInstance();

  void postEvent(AppEvent event);

  AppEvent pop();

  bool empty();

private:
  static EventQueue* instance;
  std::queue<AppEvent> eventQueue;
};

#endif
