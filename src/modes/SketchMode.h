#ifndef UDEN_SKETCH_MODE
#define UDEN_SKETCH_MODE

#include "../Event.h"
#include "../Mode.h"
#include "../ApplicationState.h"

class SketchMode : public Mode {
public:
  SketchMode();
  ~SketchMode();

  bool processEvent(AppEvent event) override;
  bool keyPress(KeyPress key) override;
  bool keyRelease(KeyPress key) override;
  bool mousePress(MouseKeyPress button) override;
  bool mouseRelease(MouseKeyPress button) override;
};

#endif
