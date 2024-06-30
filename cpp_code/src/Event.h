#ifndef UDEN_EVENT
#define UDEN_EVENT

#include "raylib.h"
#include "json.hpp"

#include "cad/ConstraintGraph.h"

#include <variant>
#include <queue>
#include <iostream>

struct enableSketchMode {};
struct disableSketchMode {};
struct toggleSketchMode {};
struct popMode {};
struct togglePointMode {};
struct toggleLineMode {};
struct toggleTLineMode {};
struct toggleExtrudeMode {};
struct toggleDimensionMode {};
struct startRotate {};
struct stopRotate {};
struct splitPaneHorizontally { Vector2 mousePos; };
struct splitPaneVertically { Vector2 mousePos; };
struct collapseBoundary { Vector2 mousePos; };
// Is subject to change in the future. Think about how hit's should be handled.
struct sketchPlaneHit { double x, y, z; Ray ray; };
// Sketch view specific events
struct sketchClick { double x, y, zoomScale; };
struct sketchConstrain { ConstraintType type; };
struct confirmDimension {};
struct dumpShapes {};
struct increaseZoom {};
struct decreaseZoom {};

struct exitProgram {};

using AppEvent = std::variant<
  enableSketchMode,
  disableSketchMode,
  toggleSketchMode,
  popMode,
  togglePointMode,
  toggleLineMode,
  toggleTLineMode,
  toggleExtrudeMode,
  toggleDimensionMode,
  startRotate,
  splitPaneHorizontally,
  splitPaneVertically,
  collapseBoundary,
  stopRotate,
  sketchPlaneHit,
  sketchClick,
  sketchConstrain,
  confirmDimension,
  dumpShapes,
  increaseZoom,
  decreaseZoom,
  exitProgram
>;

const AppEvent APP_EVENTS[] = {
  enableSketchMode {},
  disableSketchMode {},
  toggleSketchMode {},
  popMode {},
  togglePointMode {},
  toggleLineMode {},
  toggleTLineMode {},
  toggleExtrudeMode {},
  toggleDimensionMode {},
  startRotate {},
  splitPaneHorizontally {},
  splitPaneVertically {},
  collapseBoundary {},
  stopRotate {},
  sketchPlaneHit {},
  sketchClick {},
  sketchConstrain {},
  confirmDimension {},
  dumpShapes {},
  increaseZoom {},
  decreaseZoom {},
  exitProgram {}
};

const AppEvent NON_SERIALIZABLE[] = {
  startRotate {},
  splitPaneHorizontally {},
  splitPaneVertically {},
  collapseBoundary {},
  stopRotate {},
  dumpShapes {},
  increaseZoom {},
  decreaseZoom {},
  exitProgram {}
};

using json = nlohmann::json;

class EventQueue {
public:
  EventQueue();
  ~EventQueue();

  static EventQueue* getInstance();

  void postEvent(AppEvent event);
  void resetHistoryIndex();

  AppEvent pop();
  std::optional<AppEvent> getNextHistoryEvent();

  bool empty();

  size_t historySize();

  json serializeHistory();
  void deserializeHistory(json state);

private:
  static EventQueue* instance;

protected:
  std::queue<AppEvent> eventQueue;
  std::vector<AppEvent> history;

  size_t historyIndex = -1;

  bool serializable(const AppEvent& event);
};

#endif
