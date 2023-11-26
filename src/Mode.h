#ifndef UDEN_MODE
#define UDEN_MODE

#include <memory>
#include <raylib.h>
#include <vector>

#include "Renderer.h"

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
  virtual bool keyRelease(KeyPress key)=0;
};

class GlobalMode : public Mode {
public:
  GlobalMode(Renderer* renderer);
  ~GlobalMode();

  bool keyPress(KeyPress key) override;
  bool keyRelease(KeyPress key) override;

private:
  Renderer* renderer;
};

class SketchMode : public Mode {
public:
  SketchMode();
  ~SketchMode();

  bool keyPress(KeyPress key) override;
  bool keyRelease(KeyPress key) override;
};

class PointMode : public Mode {
public:
  PointMode();
  ~PointMode();

  bool keyPress(KeyPress key) override;
  bool keyRelease(KeyPress key) override;
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
};

#endif
