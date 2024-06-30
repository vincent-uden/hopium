#include "GlobalMode.h"

GlobalMode::GlobalMode() {
}

GlobalMode::~GlobalMode() {
}

bool GlobalMode::processEvent(AppEvent event) {
  ApplicationState* state = ApplicationState::getInstance();
  if (toggleSketchMode* e = std::get_if<toggleSketchMode>(&event)) {
    state->modeStack.push(state->sketch);
    state->sketchModeActive = true;
    return true;
  } else if (enableSketchMode* e = std::get_if<enableSketchMode>(&event)) {
    state->modeStack.push(state->sketch);
    state->sketchModeActive = true;
  } else if (disableSketchMode* e = std::get_if<disableSketchMode>(&event)) {
    state->modeStack.exit(state->sketch);
    state->sketchModeActive = false;
  } else if (popMode* e = std::get_if<popMode>(&event)) {
    if (state->modeStack.size() > 1) {
      state->modeStack.pop();
    }
    state->sketchModeActive = state->modeStack.isActive(state->sketch);
  } else if (startRotate* e = std::get_if<startRotate>(&event)) {
    state->holdingRotate = true;
  } else if (stopRotate* e = std::get_if<stopRotate>(&event)) {
    state->holdingRotate = false;
  } else if (splitPaneHorizontally* e = std::get_if<splitPaneHorizontally>(&event)) {
  } else if (splitPaneVertically* e = std::get_if<splitPaneVertically>(&event)) {
  } else if (collapseBoundary* e = std::get_if<collapseBoundary>(&event)) {
  } else if (dumpShapes* e = std::get_if<dumpShapes>(&event)) {
    state->occtScene->dumpShapes();
  } else if (exitProgram* e = std::get_if<exitProgram>(&event)) {
    // TODO: Move here
  } else if (increaseZoom* e = std::get_if<increaseZoom>(&event)) {
    state->zoom *= 1.25;
  } else if (decreaseZoom* e = std::get_if<decreaseZoom>(&event)) {
    state->zoom /= 1.25;
  }
  // TODO: Extrude Occt scene

  return false;
}

bool GlobalMode::keyPress(KeyPress key) {
  bool consumed = true;

  switch (key.key) {
  case KEY_H:
    EventQueue::getInstance()->postEvent(splitPaneHorizontally { GetMousePosition() });
    break;
  case KEY_V:
    EventQueue::getInstance()->postEvent(splitPaneVertically { GetMousePosition() });
    break;
  case KEY_D:
    EventQueue::getInstance()->postEvent(collapseBoundary { GetMousePosition() });
    break;
  case KEY_SPACE:
    EventQueue::getInstance()->postEvent(startRotate {});
    break;
  case KEY_S:
    EventQueue::getInstance()->postEvent(dumpShapes {});
    break;
  case KEY_Q:
    EventQueue::getInstance()->postEvent(exitProgram {});
    break;
  case KEY_X:
    EventQueue::getInstance()->postEvent(increaseZoom {});
    break;
  case KEY_Z:
    EventQueue::getInstance()->postEvent(decreaseZoom {});
    break;
  default:
    consumed = false;
  }

  return consumed;
}

bool GlobalMode::keyRelease(KeyPress key) {
  bool consumed = true;

  switch (key.key) {
  case KEY_SPACE:
    EventQueue::getInstance()->postEvent(stopRotate {});
    break;
  default:
    consumed = false;
  }

  return consumed;
}

bool GlobalMode::mousePress(MouseKeyPress button) {
  return false;
}

bool GlobalMode::mouseRelease(MouseKeyPress button) {
  return false;
}
