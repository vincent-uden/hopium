#include <iostream>

#include <memory>
#include <raylib.h>
#include <json.hpp>

#include "src/Application.h"

int main(int argc, char** argv) {

  SetConfigFlags(FLAG_MSAA_4X_HINT);
  InitWindow(1600, 900, "Raylib Example");
  SetTraceLogLevel(LOG_WARNING);
  SetTargetFPS(60);

  ApplicationState* state = ApplicationState::getInstance();
  Application* app = Application::getInstance();

  // We need to dealloc the renderer and all it's textures before closing the
  // window

  SetExitKey(-1);
  while ( !WindowShouldClose() && !app->shouldExit ) {
    app->update();
  }

  delete app;
  delete state;

  CloseWindow();

  return 0;
}
