#ifndef UDEN_UI_SKETCHVIEWER
#define UDEN_UI_SKETCHVIEWER

#include <map>
#include <string>

#include "../Ui.h"
#include "../../cad/Sketch.h"
#include "Icon.h"
#include "Text.h"

#include <raylib.h>

namespace Ui {

class SketchViewer: public Ui {
public:
  SketchViewer();
  ~SketchViewer();

  void move(Vector2 distance) override;
  void setPos(Vector2 pos) override;
  void draw() override;
  void receiveMousePos(Vector2 mousePos) override;
  void receiveMouseDown(Vector2 mousePos) override;
  void receiveMouseUp(Vector2 mousePos) override;
  void receiveMouseWheel(Vector2 mousePos, float movement) override;
  Vector2 getSize() override;

  void setAreaPointers(Rectangle* screenRect, Vector2* screenPos, RenderTexture* texture);
  void setSketch(std::shared_ptr<Sketch::NewSketch> sketch);

private:
  Vector2 toScreenSpace(Vector2 sketchPos);
  Vector2 toSketchSpace(Vector2 pos);

  void drawConstraints();
  void drawEntity(std::shared_ptr<Sketch::Point> point);
  void drawEntity(std::shared_ptr<Sketch::Line> line);
  void drawEntity(std::shared_ptr<Sketch::SketchEntity> entity);

  Vector2 pos;
  Vector2 lastMousePos;
  Vector2 panOffset;
  float scale = 200.0f;
  float zoom = 1.0f;

  std::shared_ptr<Sketch::NewSketch> sketch;

  Rectangle* areaScreenRect = nullptr;
  Vector2* areaScreenPos = nullptr;
  RenderTexture* areaTexture = nullptr;

  Text error;
  std::map<ConstraintType, std::shared_ptr<Icon>> constraintIcons;
};

}

#endif
