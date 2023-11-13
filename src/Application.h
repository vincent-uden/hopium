#ifndef UDEN_APPLICATION
#define UDEN_APPLICATION

#include <queue>
#include <variant>

#include <raylib.h>

#include "OcctTest.h"
#include "Renderer.h"
#include "Mode.h"
#include "Renderer.h"
#include "Scene.h"
#include "System.h"
#include "Ui.h"

struct enableSketchMode {};
struct disableSketchMode {};

using AppEvent = std::variant<enableSketchMode, disableSketchMode>;

class Application {
public:
  static Application* getInstance();
  ~Application();

  void postEvent(AppEvent event);
  void update();


private:
  Application();

  static Application* instance;

  std::queue<AppEvent> eventQueue;

  int screenWidth = 1600;
  int screenHeight = 900;
  std::string layoutPath;

  Renderer renderer;
  ModeStack modeStack;
};

#endif
