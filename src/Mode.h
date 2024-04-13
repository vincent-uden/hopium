#ifndef UDEN_MODE
#define UDEN_MODE

#include <memory>
#include <raylib.h>
#include <vector>
#include <ranges>

#include "Event.h"

typedef struct {
  KeyboardKey key;
  bool shift;
  bool ctrl;
  bool lAlt;
  bool rAlt;
} KeyPress;

typedef struct {
  MouseButton button;
  bool shift;
  bool ctrl;
  bool lAlt;
  bool rAlt;
} MouseKeyPress;

char keyPressToChar(KeyPress k);

// TODO: Modes should probably be able to recieve application events.
class Mode {
public:
  virtual bool processEvent(AppEvent event)=0;
  virtual bool keyPress(KeyPress key)=0;
  virtual bool keyRelease(KeyPress key)=0;
  virtual bool mousePress(MouseKeyPress button)=0;
  virtual bool mouseRelease(MouseKeyPress button)=0;
};

class ModeStack {
public:
  ModeStack();
  ~ModeStack();

  void processEvent(AppEvent event);
  void exit(std::shared_ptr<Mode> mode);
  void update();
  void push(std::shared_ptr<Mode> mode);
  void pop();

  std::shared_ptr<Mode> peek(int index);

  int size();

  bool isActive(std::shared_ptr<Mode> mode);
  bool isInnerMostMode(std::shared_ptr<Mode> mode);

private:
  std::vector<std::shared_ptr<Mode>> modes;

  std::vector<KeyboardKey> allKeys;
  std::vector<MouseButton> allMouseButtons;
};

#endif
