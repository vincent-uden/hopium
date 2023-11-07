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

  std::shared_ptr<Ui> viewport(new Ui3DViewport());
  std::shared_ptr<Scene> scene(new Scene());
  std::shared_ptr<RasterBody> utahPot(new RasterBody());
  utahPot->loadFromFile("../assets/toilet_rolls.obj");
  Texture2D utahTexture = LoadTexture("../assets/red.png");
  utahPot->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = utahTexture;
  scene->addBody(utahPot);
  std::shared_ptr<Ui3DViewport> port = std::dynamic_pointer_cast<Ui3DViewport>(viewport);
  port->setScene(scene);

  Shader shader = LoadShader("../shaders/vs.glsl", "../shaders/fs.glsl");
  utahPot->model.materials[0].shader = shader;

  // Ambient light level (some basic lighting)
  int ambientLoc = GetShaderLocation(shader, "ambientColor");
  float ambColor[3] = { 1.0f, 1.0f, 1.0f };
  SetShaderValue(shader, ambientLoc, ambColor, SHADER_UNIFORM_VEC3);

  // TODO: Update renderer camera with firstperson or orbit to figure out whats going on with the shader

  {
    // We need to dealloc the renderer and all it's textures before closing the
    // window
    Renderer renderer(screenWidth, screenHeight);
    renderer.splitPaneVertical({1, 1});
    renderer.areas[0]->addUi(fileDropDown);
    renderer.areas[1]->addUi(viewport);
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

      float cameraPos[3] = { port->camera.position.x, port->camera.position.y, port->camera.position.z };
      SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);

      UpdateCamera(&port->camera, CAMERA_ORBITAL);

      renderer.draw();
    }
  }
  UnloadShader(shader);
  CloseWindow();

  return 0;
}
