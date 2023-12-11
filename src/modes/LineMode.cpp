#include "LineMode.h"

LineMode::LineMode() {
}

LineMode::~LineMode() {
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

