#include "PointMode.h"

PointMode::PointMode() {
}

PointMode::~PointMode() {
}

bool PointMode::processEvent(AppEvent event) {
  ApplicationState* state = ApplicationState::getInstance();
  if (togglePointMode* e = std::get_if<togglePointMode>(&event)) {
    state->modeStack.exit(state->point);
    return true;
  } else if (sketchPlaneHit* e = std::get_if<sketchPlaneHit>(&event)) {
    state->occtScene->createPoint(
      state->editingSketchId,
      Vector3 {
        static_cast<float>(e->x),
        static_cast<float>(e->y),
        static_cast<float>(e->z)
      }
    );
    return true;
  } else if (sketchClick* e = std::get_if<sketchClick>(&event)) {
    Vector2 mousePos(e->x, e->y);
    std::shared_ptr<GeometricElement> elem
      = std::make_shared<GeometricElement>(GeometricType::POINT, "");
    std::shared_ptr<Sketch::Point> p = std::make_shared<Sketch::Point>(elem);
    p->pos = mousePos;
    state->paramSketch->addPoint(p);
  }

  return false;
}

bool PointMode::keyPress(KeyPress key) {
  bool consumed = true;

  switch (key.key) {
  case KEY_ESCAPE:
    EventQueue::getInstance()->postEvent(popMode {});
    break;
  case KEY_P:
    break;
  default:
    consumed = false;
  }

  return consumed;
}

bool PointMode::keyRelease(KeyPress key) {
  bool consumed = true;

  switch (key.key) {
  default:
    consumed = false;
  }

  return consumed;
}

bool PointMode::mousePress(MouseKeyPress button) {
  bool consumed = false;

  return consumed;
}

bool PointMode::mouseRelease(MouseKeyPress button) {
  bool consumed = true;
  switch (button.button) {
  case MOUSE_BUTTON_LEFT:
    EventQueue::getInstance()->postEvent(togglePointMode {});
    break;
  default:
    consumed = false;
  }

  return false;
}

