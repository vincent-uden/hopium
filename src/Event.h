#ifndef UDEN_EVENT
#define UDEN_EVENT

#include "raylib.h"
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
struct splitPaneHorizontally { Vector2 mousePos; };
struct splitPaneVertically { Vector2 mousePos; };
struct collapseBoundary { Vector2 mousePos; };
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
  splitPaneHorizontally,
  splitPaneVertically,
  collapseBoundary,
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
