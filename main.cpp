#include <BRepTools.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepMesh_Context.hxx>
#include <BRepMesh_FaceDiscret.hxx>
#include <BRepMesh_DelabellaMeshAlgoFactory.hxx>
#include <BRepMesh_ModelPostProcessor.hxx>
#include <Standard_Handle.hxx>

#include <iostream>

#include <raylib.h>

#include "Renderer.h"
#include "Ui.h"

int main(int argc, char** argv) {
  const int screenWidth = 1600;
  const int screenHeight = 900;

  InitWindow(screenWidth, screenHeight, "Raylib Example");
  SetTraceLogLevel(LOG_ERROR);

  SetTargetFPS(60);

  CLITERAL(Color) c { 31, 31, 31, 255 };

  Vector2 mousePos;

  std::vector<std::string> fileOptions;
  fileOptions.push_back("New");
  fileOptions.push_back("Open");
  fileOptions.push_back("Exit");
  std::shared_ptr<Ui> fileDropDown(new UiDropDown("File", fileOptions));
  fileDropDown->setPos({10, 10});

  std::vector<std::string> editOptions;
  editOptions.push_back("Undo");
  editOptions.push_back("Redo");
  editOptions.push_back("Preferences");
  std::shared_ptr<Ui> editDropDown(new UiDropDown("Edit", editOptions));
  editDropDown->setPos({10, 10});

  {
    // We need to dealloc the renderer and all it's textures before closing the
    // window
    Renderer renderer(screenWidth, screenHeight);
    renderer.splitPaneVertical({1, 1});
    renderer.areas[0]->addUi(fileDropDown);
    renderer.areas[1]->addUi(editDropDown);

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
