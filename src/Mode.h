#ifndef UDEN_MODE
#define UDEN_MODE

#include "Renderer.h"
#include <memory>
#include <raylib.h>
#include <vector>

typedef struct {
  KeyboardKey key;
  bool shift;
  bool ctrl;
  bool lAlt;
  bool rAlt;
} KeyPress;

class Mode {
public:
  virtual bool keyPress(KeyPress key)=0;
};

class GlobalMode : public Mode {
public:
  GlobalMode(Renderer* renderer);
  ~GlobalMode();

  bool keyPress(KeyPress key) override;

private:
  Renderer* renderer;
};

class SketchMode : public Mode {
public:
  SketchMode();
  ~SketchMode();

  bool keyPress(KeyPress key) override;
};

class ModeStack {
public:
  ModeStack();
  ~ModeStack();

  void update();
  void push(std::shared_ptr<Mode> mode);
  void pop();

private:
  std::vector<std::shared_ptr<Mode>> modes;

  std::vector<KeyboardKey> allKeys;
};

#endif