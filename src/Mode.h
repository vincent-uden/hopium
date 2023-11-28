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

class Mode {
public:
  virtual bool keyPress(KeyPress key)=0;
  virtual bool keyRelease(KeyPress key)=0;
  virtual bool mousePress(MouseKeyPress button)=0;
  virtual bool mouseRelease(MouseKeyPress button)=0;
};

class GlobalMode : public Mode {
public:
  GlobalMode();
  ~GlobalMode();

  bool keyPress(KeyPress key) override;
  bool keyRelease(KeyPress key) override;
  bool mousePress(MouseKeyPress button) override;
  bool mouseRelease(MouseKeyPress button) override;
};

class SketchMode : public Mode {
public:
  SketchMode();
  ~SketchMode();

  bool keyPress(KeyPress key) override;
  bool keyRelease(KeyPress key) override;
  bool mousePress(MouseKeyPress button) override;
  bool mouseRelease(MouseKeyPress button) override;
};

class PointMode : public Mode {
public:
  PointMode();
  ~PointMode();

  bool keyPress(KeyPress key) override;
  bool keyRelease(KeyPress key) override;
  bool mousePress(MouseKeyPress button) override;
  bool mouseRelease(MouseKeyPress button) override;
};

class ModeStack {
public:
  ModeStack();
  ~ModeStack();

  void exit(std::shared_ptr<Mode> mode);
  void update();
  void push(std::shared_ptr<Mode> mode);
  void pop();

  std::shared_ptr<Mode> peek(int index);

  int size();

  bool isActive(std::shared_ptr<Mode> mode);

private:
  std::vector<std::shared_ptr<Mode>> modes;

  std::vector<KeyboardKey> allKeys;
  std::vector<MouseButton> allMouseButtons;
};

#endif
