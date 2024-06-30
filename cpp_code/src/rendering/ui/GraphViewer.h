#ifndef UDEN_UI_GRAPHVIEWER
#define UDEN_UI_GRAPHVIEWER

#include <string>

#include "../Ui.h"
#include "../../cad/ConstraintGraph.h"

#include <raylib.h>

namespace Ui {

class GraphViewer: public Ui {
public:
  GraphViewer();
  ~GraphViewer();

  void move(Vector2 distance) override;
  void setPos(Vector2 pos) override;
  void draw() override;
  void receiveMousePos(Vector2 mousePos) override;
  void receiveMouseDown(Vector2 mousePos) override;
  void receiveMouseUp(Vector2 mousePos) override;
  void receiveMouseWheel(Vector2 mousePos, float movement) override;
  Vector2 getSize() override;

  void setAreaPointers(Rectangle* screenRect, Vector2* screenPos, RenderTexture* texture);
  void setGraph(std::shared_ptr<ConstraintGraph> graph);
  Vector2 getPos();
  Rectangle boundingBox(float padding);

private:
  Vector2 toScreenSpace(const Vector2 p);
  Vector2 toGraphSpace(const Vector2 p);

  Vector2 lastMousePos;
  Vector2 pos;
  float centralAttraction = 0.5f;
  float dt = 0.01;
  float edgeThickness = 2.f;
  float pullForce = 40.f;
  float pushForce = 0.3f;
  float scale = 200.f;
  float springLength = 0.2f;
  float selectThreshold = 15.0f;
  float velocityDamping = 0.98;
  int grabbedId = -1;
  int hoveredId = -1;
  std::shared_ptr<ConstraintGraph> graph;
  std::vector<Vector2> nodeAcc;
  std::vector<Vector2> nodePos;
  std::vector<Vector2> nodeVel;

  Rectangle* areaScreenRect = nullptr;
  Vector2* areaScreenPos = nullptr;
  RenderTexture* areaTexture = nullptr;
};

}

#endif
