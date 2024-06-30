#ifndef UDEN_LINE_MODE
#define UDEN_LINE_MODE

#include "../Event.h"
#include "../Mode.h"
#include "../ApplicationState.h"

class LineMode : public Mode {
public:
  LineMode();
  ~LineMode();

  bool processEvent(AppEvent event) override;
  bool keyPress(KeyPress key) override;
  bool keyRelease(KeyPress key) override;
  bool mousePress(MouseKeyPress button) override;
  bool mouseRelease(MouseKeyPress button) override;
};

#endif
