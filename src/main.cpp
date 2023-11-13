#include <BRepTools.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepMesh_Context.hxx>
#include <BRepMesh_FaceDiscret.hxx>
#include <BRepMesh_DelabellaMeshAlgoFactory.hxx>
#include <BRepMesh_ModelPostProcessor.hxx>
#include <Standard_Handle.hxx>

#include <iostream>

#include <memory>
#include <raylib.h>
#include <json.hpp>

#include "Application.h"

int main(int argc, char** argv) {

  SetConfigFlags(FLAG_MSAA_4X_HINT);
  InitWindow(1600, 900, "Raylib Example");
  SetTraceLogLevel(LOG_WARNING);
  SetTargetFPS(60);

  Application* app = Application::getInstance();

  // We need to dealloc the renderer and all it's textures before closing the
  // window

  while ( !WindowShouldClose() ) {
    app->update();
  }

  delete app;
  CloseWindow();

  return 0;
}
