#ifndef UDEN_APPLICATION
#define UDEN_APPLICATION

#include <raylib.h>

#include "ApplicationState.h"
#include "Event.h"
#include "Mode.h"
#include "System.h"
#include "cad/OcctScene.h"
#include "cad/OcctTest.h"
#include "modes/DimensionMode.h"
#include "modes/ExtrudeMode.h"
#include "modes/GlobalMode.h"
#include "modes/LineMode.h"
#include "modes/PointMode.h"
#include "modes/SketchMode.h"
#include "modes/TLineMode.h"
#include "rendering/Renderer.h"
#include "rendering/Scene.h"

class Application {
public:
  static Application* getInstance();
  ~Application();

  void update();

  bool shouldExit = false;

private:
  Application();

  void buildGraph();
  void buildGraph2();
  void buildSketch();
  void processEvent(enableSketchMode event);
  void processEvent(disableSketchMode event);
  void processEvent(toggleSketchMode event);
  void processEvent(popMode event);
  void processEvent(togglePointMode event);
  void processEvent(toggleLineMode event);
  void processEvent(toggleTLineMode event);
  void processEvent(toggleExtrudeMode event);
  void processEvent(toggleDimensionMode event);
  void processEvent(startRotate event);
  void processEvent(stopRotate event);
  void processEvent(splitPaneHorizontally event);
  void processEvent(splitPaneVertically event);
  void processEvent(collapseBoundary event);
  void processEvent(sketchPlaneHit event);
  void processEvent(sketchClick event);
  void processEvent(sketchConstrain event);
  void processEvent(dumpShapes event);
  void processEvent(increaseZoom event);
  void processEvent(decreaseZoom event);
  void processEvent(exitProgram event);

  static Application* instance;

  int screenWidth = 1600;
  int screenHeight = 900;
  std::string layoutPath;
  std::string scenePath;

  Renderer renderer;
  EventQueue eventQueue;

  ApplicationState* state;
};

#endif
