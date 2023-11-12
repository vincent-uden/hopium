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

#include "Mode.h"
#include "OcctTest.h"
#include "Renderer.h"
#include "Scene.h"
#include "System.h"
#include "Ui.h"

const std::string LAYOUT_PATH = "layout.json";

int main(int argc, char** argv) {
  const int screenWidth = 1600;
  const int screenHeight = 900;

  createBottle();

  SetConfigFlags(FLAG_MSAA_4X_HINT);
  InitWindow(screenWidth, screenHeight, "Raylib Example");
  SetTraceLogLevel(LOG_WARNING);

  SetTargetFPS(60);

  CLITERAL(Color) c { 31, 31, 31, 255 };

  Vector2 mousePos;

  {
    // We need to dealloc the renderer and all it's textures before closing the
    // window
    Renderer renderer(screenWidth, screenHeight);
    if (fileExists(LAYOUT_PATH)) {
      json layout = json::parse(readFromFile(LAYOUT_PATH));
      renderer.deserialize(layout);
    }

    std::shared_ptr<Mode> global(new GlobalMode(&renderer));
    std::shared_ptr<SketchMode> sketchMode(new SketchMode());
    ModeStack modeStack;
    modeStack.push(global);
    modeStack.push(sketchMode);

    while ( !WindowShouldClose() ) {
      modeStack.update();

      mousePos = GetMousePosition();
      renderer.receiveMousePos(mousePos);
      if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        renderer.mouseDown(mousePos);
      }
      if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        renderer.mouseUp(mousePos);
      }

      renderer.draw();
    }

    writeToFile(LAYOUT_PATH, renderer.serialize().dump(-1));
  }
  CloseWindow();

  return 0;
}
