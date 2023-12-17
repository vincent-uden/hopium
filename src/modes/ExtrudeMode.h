#ifndef UDEN_EXTRUDE_MODE
#define UDEN_EXTRUDE_MODE

#include "../Event.h"
#include "../Mode.h"

class ExtrudeMode : public Mode {
public:
  ExtrudeMode();
  ~ExtrudeMode();

  bool keyPress(KeyPress key) override;
  bool keyRelease(KeyPress key) override;
  bool mousePress(MouseKeyPress button) override;
  bool mouseRelease(MouseKeyPress button) override;
};

#endif
