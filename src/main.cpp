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

int main(int argc, char** argv) {
  const int screenWidth = 1600;
  const int screenHeight = 900;

  SetConfigFlags(FLAG_MSAA_4X_HINT);
  InitWindow(screenWidth, screenHeight, "Raylib Example");
  SetTraceLogLevel(LOG_WARNING);

  SetTargetFPS(60);

  CLITERAL(Color) c { 31, 31, 31, 255 };

  Vector2 mousePos;

  std::vector<std::string> fileOptions;
  fileOptions.push_back("New");
  fileOptions.push_back("Open");
  fileOptions.push_back("Save");
  fileOptions.push_back("Save as");
  fileOptions.push_back("Exit");
  std::shared_ptr<Ui> fileDropDown(new UiDropDown("File", fileOptions));
  fileDropDown->setPos({0, 0});

  {
    // We need to dealloc the renderer and all it's textures before closing the
    // window
    Renderer renderer(screenWidth, screenHeight);
    renderer.splitPaneHorizontal({1, 1});
    renderer.areas[0]->addUi(fileDropDown);
    renderer.areas[1]->anchor = RenderAnchor::CENTER;

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

      // TODO: Move to renderer
      float cameraPos[3] = { port->camera.position.x, port->camera.position.y, port->camera.position.z };
      SetShaderValue(Scene::standardModelShader, Scene::standardModelShader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);

      UpdateCamera(&port->camera, CAMERA_ORBITAL);
      // --

      renderer.draw();
    }
  }
  UnloadShader(Scene::standardModelShader);
  CloseWindow();

  return 0;
}
