#include "LineMode.h"

LineMode::LineMode() {
}

LineMode::~LineMode() {
}

bool LineMode::processEvent(AppEvent event) {
  ApplicationState* state = ApplicationState::getInstance();
  if (toggleLineMode* e = std::get_if<toggleLineMode>(&event)) {
    state->modeStack.exit(state->line);
    return true;
  } else if (sketchPlaneHit* e = std::get_if<sketchPlaneHit>(&event)) {
    std::optional<std::shared_ptr<RasterVertex>> p = state->scene->queryVertex(e->ray, state->selectionThreshold);
    if (p.has_value()) {
      e->x = p.value()->x;
      e->y = p.value()->y;
      e->z = p.value()->z;
    }
    if (state->activePoints.size() < 1) {
      state->activePoints.push_back(gp_Pnt(e->x, e->y, e->z));
    } else {
      state->activePoints.push_back(gp_Pnt(e->x, e->y, e->z));
      //state->occtScene->createLine(state->activePoints[0], state->activePoints[1], 1e-5);
      state->scene->setShapes(state->occtScene->rasterizeShapes());
      state->activePoints.clear();
      EventQueue::getInstance()->postEvent(toggleLineMode {});
    }
    return true;
  } else if (sketchClick* e = std::get_if<sketchClick>(&event)) {
    Vector2 mousePos(e->x, e->y);
    state->activeCoordinates.push_back(mousePos);
    if (state->activeCoordinates.size() == 2) {
      std::shared_ptr<GeometricElement> e =
        std::make_shared<GeometricElement>(GeometricType::LINE, "");
      std::shared_ptr<Sketch::Line> l = std::make_shared<Sketch::Line>(e);

      Vector2 c0 = state->activeCoordinates[0];
      Vector2 c1 = state->activeCoordinates[1];
      l->k = (c1.y-c0.y)/(c1.x-c0.x);
      l->m = -c0.x * l->k + c0.y;
      state->paramSketch->addLine(l);
      state->activeCoordinates.clear();
      EventQueue::getInstance()->postEvent(popMode {});
      return true;
    }
  }

  return false;
}

bool LineMode::keyPress(KeyPress key) {
  bool consumed = true;

  switch (key.key) {
  case KEY_ESCAPE:
    EventQueue::getInstance()->postEvent(popMode {});
    break;
  case KEY_L:
    break;
  default:
    consumed = false;
  }

  return consumed;
}

bool LineMode::keyRelease(KeyPress key) {
  bool consumed = true;

  switch (key.key) {
  default:
    consumed = false;
  }

  return consumed;
}

bool LineMode::mousePress(MouseKeyPress button) {
  bool consumed = false;

  return consumed;
}

bool LineMode::mouseRelease(MouseKeyPress button) {
  bool consumed = true;
  switch (button.button) {
  case MOUSE_BUTTON_LEFT:
    break;
  default:
    consumed = false;
  }

  return false;
}

