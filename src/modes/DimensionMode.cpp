#include "DimensionMode.h"

DimensionMode::DimensionMode() {
}

DimensionMode::~DimensionMode() {
}

bool DimensionMode::processEvent(AppEvent event) {
  ApplicationState* state = ApplicationState::getInstance();
  if (toggleDimensionMode* e = std::get_if<toggleDimensionMode>(&event)) {
    state->modeStack.exit(state->dimension);
    return true;
  }

  return false;
}

bool DimensionMode::keyPress(KeyPress key) {
  bool consumed = true;
  ApplicationState* state = ApplicationState::getInstance();

  switch (key.key) {
  case KEY_ESCAPE:
    EventQueue::getInstance()->postEvent(popMode {});
    break;
  default:
    consumed = false;
  }

  if (key.key == KEY_DELETE || key.key == KEY_BACKSPACE) {
    state->pendingDimension.pop_back();
  } else if (char c = keyPressToChar(key))  {
    state->pendingDimension.push_back(c);
  } else if (key.key == KEY_LEFT) {
    state->pendingDimCursor = std::max(state->pendingDimCursor - 1, 0);
  } else if (key.key == KEY_RIGHT) {
    state->pendingDimCursor = std::min(
      state->pendingDimCursor + 1,
      static_cast<int>(state->pendingDimension.size())
    );
  }

  return consumed;
}

bool DimensionMode::keyRelease(KeyPress key) {
  bool consumed = true;

  switch (key.key) {
  default:
    consumed = false;
  }

  return consumed;
}

bool DimensionMode::mousePress(MouseKeyPress button) {
  bool consumed = false;

  return consumed;
}

bool DimensionMode::mouseRelease(MouseKeyPress button) {
  bool consumed = true;
  switch (button.button) {
  case MOUSE_BUTTON_LEFT:
    EventQueue::getInstance()->postEvent(toggleDimensionMode {});
    break;
  default:
    consumed = false;
  }

  return false;
}

