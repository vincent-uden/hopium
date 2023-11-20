#include "Mode.h"

GlobalMode::GlobalMode(Renderer* renderer) {
  this->renderer = renderer;
}

GlobalMode::~GlobalMode() {
}

bool GlobalMode::keyPress(KeyPress key) {
  bool consumed = true;

  switch (key.key) {
  case KEY_H:
    renderer->splitPaneHorizontal(GetMousePosition());
    break;
  case KEY_V:
    renderer->splitPaneVertical(GetMousePosition());
    break;
  case KEY_D:
    renderer->collapseBoundary(GetMousePosition());
    break;
  case KEY_SPACE:
    renderer->dumpPanes();
    break;
  default:
    consumed = false;
  }

  return consumed;
}

SketchMode::SketchMode() {
}

SketchMode::~SketchMode() {
}

bool SketchMode::keyPress(KeyPress key) {
  bool consumed = true;

  switch (key.key) {
  default:
    consumed = false;
  }

  return consumed;
}

ModeStack::ModeStack() {
    allKeys.push_back(KEY_NULL);
    allKeys.push_back(KEY_APOSTROPHE);
    allKeys.push_back(KEY_COMMA);
    allKeys.push_back(KEY_MINUS);
    allKeys.push_back(KEY_PERIOD);
    allKeys.push_back(KEY_SLASH);
    allKeys.push_back(KEY_ZERO);
    allKeys.push_back(KEY_ONE);
    allKeys.push_back(KEY_TWO);
    allKeys.push_back(KEY_THREE);
    allKeys.push_back(KEY_FOUR);
    allKeys.push_back(KEY_FIVE);
    allKeys.push_back(KEY_SIX);
    allKeys.push_back(KEY_SEVEN);
    allKeys.push_back(KEY_EIGHT);
    allKeys.push_back(KEY_NINE);
    allKeys.push_back(KEY_SEMICOLON);
    allKeys.push_back(KEY_EQUAL);
    allKeys.push_back(KEY_A);
    allKeys.push_back(KEY_B);
    allKeys.push_back(KEY_C);
    allKeys.push_back(KEY_D);
    allKeys.push_back(KEY_E);
    allKeys.push_back(KEY_F);
    allKeys.push_back(KEY_G);
    allKeys.push_back(KEY_H);
    allKeys.push_back(KEY_I);
    allKeys.push_back(KEY_J);
    allKeys.push_back(KEY_K);
    allKeys.push_back(KEY_L);
    allKeys.push_back(KEY_M);
    allKeys.push_back(KEY_N);
    allKeys.push_back(KEY_O);
    allKeys.push_back(KEY_P);
    allKeys.push_back(KEY_Q);
    allKeys.push_back(KEY_R);
    allKeys.push_back(KEY_S);
    allKeys.push_back(KEY_T);
    allKeys.push_back(KEY_U);
    allKeys.push_back(KEY_V);
    allKeys.push_back(KEY_W);
    allKeys.push_back(KEY_X);
    allKeys.push_back(KEY_Y);
    allKeys.push_back(KEY_Z);
    allKeys.push_back(KEY_LEFT_BRACKET);
    allKeys.push_back(KEY_BACKSLASH);
    allKeys.push_back(KEY_RIGHT_BRACKET);
    allKeys.push_back(KEY_GRAVE);
    allKeys.push_back(KEY_SPACE);
    allKeys.push_back(KEY_ESCAPE);
    allKeys.push_back(KEY_ENTER);
    allKeys.push_back(KEY_TAB);
    allKeys.push_back(KEY_BACKSPACE);
    allKeys.push_back(KEY_INSERT);
    allKeys.push_back(KEY_DELETE);
    allKeys.push_back(KEY_RIGHT);
    allKeys.push_back(KEY_LEFT);
    allKeys.push_back(KEY_DOWN);
    allKeys.push_back(KEY_UP);
    allKeys.push_back(KEY_PAGE_UP);
    allKeys.push_back(KEY_PAGE_DOWN);
    allKeys.push_back(KEY_HOME);
    allKeys.push_back(KEY_END);
    allKeys.push_back(KEY_CAPS_LOCK);
    allKeys.push_back(KEY_SCROLL_LOCK);
    allKeys.push_back(KEY_NUM_LOCK);
    allKeys.push_back(KEY_PRINT_SCREEN);
    allKeys.push_back(KEY_PAUSE);
    allKeys.push_back(KEY_F1);
    allKeys.push_back(KEY_F2);
    allKeys.push_back(KEY_F3);
    allKeys.push_back(KEY_F4);
    allKeys.push_back(KEY_F5);
    allKeys.push_back(KEY_F6);
    allKeys.push_back(KEY_F7);
    allKeys.push_back(KEY_F8);
    allKeys.push_back(KEY_F9);
    allKeys.push_back(KEY_F10);
    allKeys.push_back(KEY_F11);
    allKeys.push_back(KEY_F12);
    allKeys.push_back(KEY_LEFT_SHIFT);
    allKeys.push_back(KEY_LEFT_CONTROL);
    allKeys.push_back(KEY_LEFT_ALT);
    allKeys.push_back(KEY_LEFT_SUPER);
    allKeys.push_back(KEY_RIGHT_SHIFT);
    allKeys.push_back(KEY_RIGHT_CONTROL);
    allKeys.push_back(KEY_RIGHT_ALT);
    allKeys.push_back(KEY_RIGHT_SUPER);
    allKeys.push_back(KEY_KB_MENU);
    allKeys.push_back(KEY_KP_0);
    allKeys.push_back(KEY_KP_1);
    allKeys.push_back(KEY_KP_2);
    allKeys.push_back(KEY_KP_3);
    allKeys.push_back(KEY_KP_4);
    allKeys.push_back(KEY_KP_5);
    allKeys.push_back(KEY_KP_6);
    allKeys.push_back(KEY_KP_7);
    allKeys.push_back(KEY_KP_8);
    allKeys.push_back(KEY_KP_9);
    allKeys.push_back(KEY_KP_DECIMAL);
    allKeys.push_back(KEY_KP_DIVIDE);
    allKeys.push_back(KEY_KP_MULTIPLY);
    allKeys.push_back(KEY_KP_SUBTRACT);
    allKeys.push_back(KEY_KP_ADD);
    allKeys.push_back(KEY_KP_ENTER);
    allKeys.push_back(KEY_KP_EQUAL);
    allKeys.push_back(KEY_BACK);
    allKeys.push_back(KEY_MENU);
    allKeys.push_back(KEY_VOLUME_UP);
    allKeys.push_back(KEY_VOLUME_DOWN);
}

ModeStack::~ModeStack() {
}

void ModeStack::exit(std::shared_ptr<Mode> mode) {
  bool foundMode = false;
  for (auto it = modes.rbegin(); it != modes.rend(); ++it) {
    if (it->get() == mode.get()) {
      foundMode = true;
    }
  }

  if (foundMode) {
    for (auto it = modes.rbegin(); it != modes.rend(); ++it) {
      if (it->get() == mode.get()) {
        modes.pop_back();
        break;
      }
      modes.pop_back();
    }
  }
}

// Fetch all key presses and pass them down the mode stack. The first modes get
// precedence on key presses.
void ModeStack::update() {
  bool shift = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
  bool ctrl = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
  bool lAlt = IsKeyDown(KEY_LEFT_ALT);
  bool rAlt = IsKeyDown(KEY_RIGHT_ALT);

  for (KeyboardKey key : allKeys) {
    if (IsKeyPressed(key)) {
      KeyPress press = { key, shift, ctrl, lAlt, rAlt };

      for (auto& mode : modes) {
        if (mode->keyPress(press)) {
          break;
        }
      }
    }
  }
}

void ModeStack::push(std::shared_ptr<Mode> mode) {
  modes.push_back(mode);
}

void ModeStack::pop() {
  modes.pop_back();
}

std::shared_ptr<Mode> ModeStack::peek(int index) {
  if (index >= 0 && index < modes.size()) {
    return modes[index];
  }

  return nullptr;
}


int ModeStack::size() {
  return modes.size();
}
