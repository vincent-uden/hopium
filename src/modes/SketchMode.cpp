#include "SketchMode.h"
#include "raylib.h"

SketchMode::SketchMode() {
}

SketchMode::~SketchMode() {
}

bool SketchMode::processEvent(AppEvent event) {
  ApplicationState* state = ApplicationState::getInstance();
  if (toggleSketchMode* e = std::get_if<toggleSketchMode>(&event)) {
    state->modeStack.exit(state->sketch);
    state->sketchModeActive = false;
    return true;
  } else if (togglePointMode* e = std::get_if<togglePointMode>(&event)) {
    state->modeStack.push(state->point);
    return true;
  } else if (toggleLineMode* e = std::get_if<toggleLineMode>(&event)) {
    state->modeStack.push(state->line);
    return true;
  } else if (toggleTLineMode* e = std::get_if<toggleTLineMode>(&event)) {
    state->modeStack.push(state->tline);
    return true;
  } else if (toggleExtrudeMode* e = std::get_if<toggleExtrudeMode>(&event)) {
    state->modeStack.push(state->extrude);
    return true;
  } else if (toggleDimensionMode* e = std::get_if<toggleDimensionMode>(&event)) {
    state->modeStack.push(state->dimension);
    return true;
  } else if (sketchClick* e = std::get_if<sketchClick>(&event)) {
    Vector2 mousePos(e->x, e->y);
    std::shared_ptr<Sketch::SketchEntity> clicked =
      state->paramSketch->findEntityByPositionGeneric(
        mousePos,
        std::pow(20.0 / e->zoomScale, 2.0)
      );
      if (IsKeyDown(KEY_LEFT_SHIFT) && clicked) {
        state->addActive(clicked);
      } else if (clicked) {
        state->setActive(clicked);
      } else {
        state->clearActive();
      }
    return true;
  } else if (sketchConstrain* e = std::get_if<sketchConstrain>(&event)) {
    std::shared_ptr<Constraint> c = std::make_shared<Constraint>(e->type);
    switch (e->type) {
      case ConstraintType::ANGLE:
        break;
      case ConstraintType::COINCIDENT:
        if (state->activeEntities.size() == 2) {
          state->paramSketch->connect(state->activeEntities[0], state->activeEntities[1], c);
          state->paramSketch->solve();
          state->clearActive();
        }
        break;
      case ConstraintType::COLINEAR:
        break;
      case ConstraintType::DISTANCE:
        if (state->activeEntities.size() == 2) {
          state->modeStack.push(state->dimension);
        }
        break;
      case ConstraintType::EQUAL:
        break;
      case ConstraintType::HORIZONTAL:
        if (state->activeEntities.size() == 2) {
          state->paramSketch->connect(state->activeEntities[0], state->activeEntities[1], c);
          state->paramSketch->solve();
          state->clearActive();
        }
        break;
      case ConstraintType::MIDPOINT:
        break;
      case ConstraintType::PARALLEL:
        break;
      case ConstraintType::PERPENDICULAR:
        break;
      case ConstraintType::VERTICAL:
        if (state->activeEntities.size() == 2) {
          state->paramSketch->connect(state->activeEntities[0], state->activeEntities[1], c);
          state->paramSketch->solve();
          state->activeEntities.clear();
        }
        break;
      case ConstraintType::VIRTUAL:
        break;
    }
  } else if (confirmDimension* e = std::get_if<confirmDimension>(&event)) {
    if (state->activeEntities.size() == 2) {
      std::shared_ptr<Constraint> c = std::make_shared<Constraint>(ConstraintType::DISTANCE);
      float dist;
      try {
        dist = std::stof(state->pendingDimension);
      } catch (std::invalid_argument e) {
        dist = 1.0;
      }
      c->value = dist;
      state->paramSketch->connect(state->activeEntities[0], state->activeEntities[1], c);
      state->paramSketch->solve();
      state->clearActive();
      state->modeStack.pop();
    }
  }

  return false;
}

bool SketchMode::keyPress(KeyPress key) {
  bool consumed = true;

  switch (key.key) {
  case KEY_ESCAPE:
    EventQueue::getInstance()->postEvent(popMode {});
    break;
  case KEY_L:
    EventQueue::getInstance()->postEvent(toggleLineMode {});
    break;
  case KEY_P:
    EventQueue::getInstance()->postEvent(togglePointMode {});
    break;
  case KEY_E:
    EventQueue::getInstance()->postEvent(toggleExtrudeMode {});
    break;
  case KEY_D:
    EventQueue::getInstance()->postEvent(toggleDimensionMode {});
    break;
  default:
    consumed = false;
  }

  return consumed;
}

bool SketchMode::keyRelease(KeyPress key) {
  bool consumed = true;

  switch (key.key) {
  default:
    consumed = false;
  }

  return consumed;
}

bool SketchMode::mousePress(MouseKeyPress button) {
  return false;
}

bool SketchMode::mouseRelease(MouseKeyPress button) {
  return false;
}
