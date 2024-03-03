#include "SketchViewer.h"

namespace Ui {

SketchViewer::SketchViewer() {
  pos.x = 0;
  pos.y = 0;
  panOffset.x = 0;
  panOffset.y = 0;

  error.setAlignment(TextAlignment::RIGHT);
  error.setColor(WHITE);
  error.setPos(pos);

  std::shared_ptr<Icon> coincident = std::make_shared<Icon>();
  coincident->setImgPath("../assets/icons/Coincident.png");
  coincident->setHoverTooltip("Coincident");
  coincident->setBgColor({ 0, 0, 0, 0 });
  std::shared_ptr<Icon> colinear = std::make_shared<Icon>();
  colinear->setImgPath("../assets/icons/Colinear.png");
  colinear->setHoverTooltip("Colinear");
  colinear->setBgColor({ 0, 0, 0, 0 });
  std::shared_ptr<Icon> equal = std::make_shared<Icon>();
  equal->setImgPath("../assets/icons/Equal.png");
  equal->setHoverTooltip("Equal");
  equal->setBgColor({ 0, 0, 0, 0 });
  std::shared_ptr<Icon> midpoint = std::make_shared<Icon>();
  midpoint->setImgPath("../assets/icons/Midpoint.png");
  midpoint->setHoverTooltip("Midpoint");
  midpoint->setBgColor({ 0, 0, 0, 0 });
  std::shared_ptr<Icon> parallel = std::make_shared<Icon>();
  parallel->setImgPath("../assets/icons/Parallel.png");
  parallel->setHoverTooltip("Parallel");
  parallel->setBgColor({ 0, 0, 0, 0 });
  std::shared_ptr<Icon> perpendicular = std::make_shared<Icon>();
  perpendicular->setImgPath("../assets/icons/Perpendicular.png");
  perpendicular->setHoverTooltip("Perpendicular");
  perpendicular->setBgColor({ 0, 0, 0, 0 });
  std::shared_ptr<Icon> vertical = std::make_shared<Icon>();
  vertical->setImgPath("../assets/icons/Vertical.png");
  vertical->setHoverTooltip("Vertical");
  vertical->setBgColor({ 0, 0, 0, 0 });
  std::shared_ptr<Icon> horizontal = std::make_shared<Icon>();
  horizontal->setImgPath("../assets/icons/Horizontal.png");
  horizontal->setHoverTooltip("Horizontal");
  horizontal->setBgColor({ 0, 0, 0, 0 });

  constraintIcons[ConstraintType::COINCIDENT] = coincident;
  constraintIcons[ConstraintType::COLINEAR] = colinear;
  constraintIcons[ConstraintType::EQUAL] = equal;
  constraintIcons[ConstraintType::MIDPOINT] = midpoint;
  constraintIcons[ConstraintType::PARALLEL] = parallel;
  constraintIcons[ConstraintType::PERPENDICULAR] = perpendicular;
  constraintIcons[ConstraintType::VERTICAL] = vertical;
  constraintIcons[ConstraintType::HORIZONTAL] = horizontal;
}

SketchViewer::~SketchViewer() {
}

void SketchViewer::move(Vector2 distance) {
  pos.x += distance.x;
  pos.y += distance.y;

  error.move(distance);
  for (auto& icon: constraintIcons) {
    icon.second->move(distance);
  }
}

void SketchViewer::setPos(Vector2 pos) {
    Vector2 diff;
    diff.x = pos.x - this->pos.x;
    diff.y = pos.y - this->pos.y;
    move(diff);
}

void SketchViewer::draw() {
  if (IsKeyDown(KEY_ENTER)) {
    sketch->sgdStep();
  }

  DrawLineV({0.0, panOffset.y}, {areaScreenRect->width, panOffset.y}, {255, 255, 255, 100});
  DrawLineV({panOffset.x, 0}, {panOffset.x, areaScreenRect->height}, {255, 255, 255, 100});

  for (const std::shared_ptr<Sketch::SketchEntity>& e: sketch->points) {
    drawEntity(e);
  }

  drawConstraints();

  error.setPos({ areaScreenRect->width - 12, 0.0  });
  error.setText(std::format("Error: {}", sketch->totalError()));
  error.draw();
}

void SketchViewer::receiveMousePos(Vector2 mousePos) {
  if (ApplicationState::getInstance()->holdingRotate) {
    Vector2 diff = Vector2Subtract(mousePos, lastMousePos);
    panOffset.x += diff.x;
    panOffset.y += diff.y;
  }

  lastMousePos = mousePos;
}

void SketchViewer::receiveMouseDown(Vector2 mousePos) {
}

void SketchViewer::receiveMouseUp(Vector2 mousePos) {
}

void SketchViewer::receiveMouseWheel(Vector2 mousePos, float movement) {
  if (movement > 0.5f) {
    zoom *= 1.1;
  } else if (movement < -0.5f) {
    zoom /= 1.1;
  }
}

Vector2 SketchViewer::getSize() {
  return { areaScreenRect->width, areaScreenRect->height };
}

void SketchViewer::setAreaPointers(
  Rectangle* screenRect,
  Vector2* screenPos,
  RenderTexture* texture
) {
  areaScreenRect = screenRect;
  areaScreenPos = screenPos;
  areaTexture = texture;

  panOffset = { areaScreenRect->width / 2.0f, areaScreenRect->height / 2.0f };
}

void SketchViewer::setSketch(std::shared_ptr<Sketch::NewSketch> sketch) {
  this->sketch = sketch;
}

Vector2 SketchViewer::toScreenSpace(Vector2 sketchPos) {
    return Vector2Add(Vector2Scale(sketchPos, scale * zoom), panOffset);
}

Vector2 SketchViewer::toSketchSpace(Vector2 pos) {
  return Vector2Scale(Vector2Subtract(pos, panOffset), 1.0/(scale * zoom));
}

void SketchViewer::drawConstraints() {
  std::vector<int> drawnIds;

  for (const auto& p1: sketch->points) {
    for (const auto& [edge, other]: p1->v->edges) {
      bool foundId = false;
      for (int& id: drawnIds) {
        if (id == edge->id) {
          foundId = true;
        }
      }
      if (!foundId) {
        std::shared_ptr<Sketch::SketchEntity> p2 = sketch->findEntityById(other);
        // TODO
        drawnIds.push_back(edge->id);
      }

    }
  }
}

void SketchViewer::drawEntity(std::shared_ptr<Sketch::Point> p) {
    Vector2 v = Vector2Add(Vector2Scale(p->pos, scale * zoom), panOffset);
    DrawCircleV(
      toScreenSpace(p->pos),
      4.0,
      GREEN
    );
}

void SketchViewer::drawEntity(std::shared_ptr<Sketch::Line> line) {
  Vector2 fullSize = { areaScreenRect->width, areaScreenRect->height};
  Vector2 start = toSketchSpace({0,0});
  Vector2 end = toSketchSpace(fullSize);
  start.y = line->k * start.x + line->m;
  end.y = line->k * end.x + line->m;
  start = toScreenSpace(start);
  end = toScreenSpace(end);
  DrawLineV(start, end, RED);
}

void SketchViewer::drawEntity(std::shared_ptr<Sketch::SketchEntity> entity) {
  if (std::shared_ptr<Sketch::Point> e1 = std::dynamic_pointer_cast<Sketch::Point>(entity); e1) {
    drawEntity(e1);
  } else if (std::shared_ptr<Sketch::Line> e1 = std::dynamic_pointer_cast<Sketch::Line>(entity); e1) {
    drawEntity(e1);
  }
}


}
