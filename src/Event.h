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
struct startRotate {};
struct stopRotate {};
// Is subject to change in the future. Think about how hit's should be handled.
struct groundPlaneHit { double x, y, z; };

struct exitProgram {};

using AppEvent = std::variant<
  enableSketchMode,
  disableSketchMode,
  toggleSketchMode,
  popMode,
  togglePointMode,
  startRotate,
  stopRotate,
  groundPlaneHit,
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
