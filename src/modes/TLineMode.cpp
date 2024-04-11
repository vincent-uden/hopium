#include "TLineMode.h"

TLineMode::TLineMode() {
}

TLineMode::~TLineMode() {
}

bool TLineMode::keyPress(KeyPress key) {
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

bool TLineMode::keyRelease(KeyPress key) {
  bool consumed = true;

  switch (key.key) {
  default:
    consumed = false;
  }

  return consumed;
}

bool TLineMode::mousePress(MouseKeyPress button) {
  bool consumed = false;

  return consumed;
}

bool TLineMode::mouseRelease(MouseKeyPress button) {
  bool consumed = true;
  switch (button.button) {
  case MOUSE_BUTTON_LEFT:
    break;
  default:
    consumed = false;
  }

  return false;
}

