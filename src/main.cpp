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

#include "Renderer.h"
#include "Scene.h"
#include "Ui.h"
#include "OcctTest.h"

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

    while ( !WindowShouldClose() ) {
      if (IsKeyPressed(KEY_H)) {
        renderer.splitPaneHorizontal(GetMousePosition());
      }
      if (IsKeyPressed(KEY_V)) {
        renderer.splitPaneVertical(GetMousePosition());
      }
      // On space, dump all pane data
      if (IsKeyPressed(KEY_SPACE)) {
        renderer.dumpPanes();
      }
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
  }
  CloseWindow();

  return 0;
}
