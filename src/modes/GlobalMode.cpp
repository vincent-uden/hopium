#include "GlobalMode.h"

GlobalMode::GlobalMode() {
}

GlobalMode::~GlobalMode() {
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
