#ifndef UDEN_APPLICATION
#define UDEN_APPLICATION

#include <raylib.h>

#include "ApplicationState.h"
#include "Event.h"
#include "Mode.h"
#include "modes/GlobalMode.h"
#include "modes/LineMode.h"
#include "modes/PointMode.h"
#include "modes/SketchMode.h"
#include "OcctScene.h"
#include "OcctTest.h"
#include "Renderer.h"
#include "Scene.h"
#include "System.h"

class Application {
public:
  static Application* getInstance();
  ~Application();

  void update();

  bool shouldExit = false;

private:
  Application();

  void processEvent(enableSketchMode event);
  void processEvent(disableSketchMode event);
  void processEvent(toggleSketchMode event);
  void processEvent(popMode event);
  void processEvent(togglePointMode event);
  void processEvent(toggleLineMode event);
  void processEvent(startRotate event);
  void processEvent(stopRotate event);
  void processEvent(splitPaneHorizontally event);
  void processEvent(splitPaneVertically event);
  void processEvent(collapseBoundary event);
  void processEvent(groundPlaneHit event);
  void processEvent(dumpShapes event);
  void processEvent(exitProgram event);

  static Application* instance;

  int screenWidth = 1600;
  int screenHeight = 900;
  std::string layoutPath;

  Renderer renderer;
  EventQueue eventQueue;

  ApplicationState* state;
};

#endif
