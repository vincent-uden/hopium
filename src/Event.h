#ifndef UDEN_EVENT
#define UDEN_EVENT

#include <variant>
#include <queue>
#include <iostream>

struct enableSketchMode {};
struct disableSketchMode {};
struct toggleSketchMode {};

using AppEvent = std::variant<
  enableSketchMode,
  disableSketchMode,
  toggleSketchMode
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
