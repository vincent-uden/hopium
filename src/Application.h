#ifndef UDEN_APPLICATION
#define UDEN_APPLICATION


#include <raylib.h>

#include "ApplicationState.h"
#include "Event.h"
#include "OcctTest.h"
#include "Renderer.h"
#include "Mode.h"
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
  void processEvent(exitProgram event);

  static Application* instance;

  int screenWidth = 1600;
  int screenHeight = 900;
  std::string layoutPath;

  Renderer renderer;
  ModeStack modeStack;
  EventQueue eventQueue;

  std::shared_ptr<Mode> global;
  std::shared_ptr<Mode> sketch;

  ApplicationState* state;
};

#endif
