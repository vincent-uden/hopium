#include "SketchMode.h"

SketchMode::SketchMode() {
}

SketchMode::~SketchMode() {
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

