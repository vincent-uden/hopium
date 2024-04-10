#ifndef UDEN_UI_VIEWPORT
#define UDEN_UI_VIEWPORT

#include <string>
#include <cfloat>

#include  "../../ApplicationState.h"
#include "../Ui.h"
#include "../Scene.h"

#include <raylib.h>

namespace Ui {

class Viewport: public Ui {
public:
  Viewport();
  ~Viewport();

  void move(Vector2 distance) override;
  void setPos(Vector2 pos) override;
  void draw() override;
  void receiveMousePos(Vector2 mousePos) override;
  void receiveMouseDown(Vector2 mousePos) override;
  void receiveMouseUp(Vector2 mousePos) override;
  void receiveMouseWheel(Vector2 mousePos, float movement) override;
  Vector2 getSize() override;

  void setScene(std::shared_ptr<Scene> scene);
  void setAreaPointers(Rectangle* screenRect, Vector2* screenPos, RenderTexture* texture);

  Camera3D camera = { 0 };
private:

  Ray getNonOffsetMouseRay(Vector2 mousePos);

  std::shared_ptr<Scene> scene;

  Rectangle* areaSreenRect = nullptr;
  Vector2* areaScreenPos = nullptr;
  RenderTexture* areaTexture = nullptr;

  // Ground Quad
  Vector3 g0 = { -20.0f, 0.0f, -20.0f };
  Vector3 g1 = { -20.0f, 0.0f,  20.0f };
  Vector3 g2 = {  20.0f, 0.0f,  20.0f };
  Vector3 g3 = {  20.0f, 0.0f, -20.0f };

  // Coordinate Axes
  Vector3 origin = { 0.0f, 0.0f, 0.0f };
  Vector3 xAxis = { 1.0f, 0.0f, 0.0f };
  Vector3 yAxis = { 0.0f, 1.0f, 0.0f };
  Vector3 zAxis = { 0.0f, 0.0f, 1.0f };

  Vector2 lastMousePos = { 0, 0 };
  double cameraSensetivity = 0.01;
  double cameraRadius = 34.64;

  Ray lastRay = { 0 };
  int hoveredId = -1;
  float lastDist = FLT_MAX;
};

}

#endif
