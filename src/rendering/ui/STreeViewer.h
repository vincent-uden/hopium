#ifndef UDEN_UI_STREEVIEWER
#define UDEN_UI_STREEVIEWER

#include <string>

#include  "../../ApplicationState.h"
#include "../Ui.h"
#include "./GraphViewer.h"

#include <raylib.h>

namespace Ui {
class STreeViewer: public Ui {
public:
  STreeViewer();
  ~STreeViewer();

  void move(Vector2 distance) override;
  void setPos(Vector2 pos) override;
  void draw() override;
  void receiveMousePos(Vector2 mousePos) override;
  void receiveMouseDown(Vector2 mousePos) override;
  void receiveMouseUp(Vector2 mousePos) override;
  void receiveMouseWheel(Vector2 mousePos, float movement) override;
  Vector2 getSize() override;

  void setAreaPointers(Rectangle* screenRect, Vector2* screenPos, RenderTexture* texture);
  void setSTree(std::shared_ptr<STree> stree);

private:
  void traverse(std::shared_ptr<STree> stree, Vector2 offset);
  void setZoom();

  Vector2 pos;
  Vector2 lastMousePos;
  Vector2 panOffset;
  float xOffset = 50.0f;
  float yOffset = 250.0f;
  // TODO: Don't do it like this
  float lastZoom = 1.0f;
  std::shared_ptr<STree> stree;
  std::vector<std::shared_ptr<GraphViewer>> nodes;

  Rectangle* areaScreenRect = nullptr;
  Vector2* areaScreenPos = nullptr;
  RenderTexture* areaTexture = nullptr;
};
}

#endif
