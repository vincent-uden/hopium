#include "ExtrudeMode.h"

ExtrudeMode::ExtrudeMode() {
}

ExtrudeMode::~ExtrudeMode() {
}

bool ExtrudeMode::keyPress(KeyPress key) {
  bool consumed = true;

  switch (key.key) {
  case KEY_ESCAPE:
    EventQueue::getInstance()->postEvent(popMode {});
    break;
  case KEY_E:
    break;
  default:
    consumed = false;
  }

  return consumed;
}

bool ExtrudeMode::keyRelease(KeyPress key) {
  bool consumed = true;

  switch (key.key) {
  default:
    consumed = false;
  }

  return consumed;
}

bool ExtrudeMode::mousePress(MouseKeyPress button) {
  bool consumed = false;

  return consumed;
}

bool ExtrudeMode::mouseRelease(MouseKeyPress button) {
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

