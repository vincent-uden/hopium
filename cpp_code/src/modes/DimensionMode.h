#ifndef UDEN_DIMENSION_MODE
#define UDEN_DIMENSION_MODE

#include "../Event.h"
#include "../Mode.h"
#include "../ApplicationState.h"

class DimensionMode : public Mode {
public:
  DimensionMode();
  ~DimensionMode();

  bool processEvent(AppEvent event) override;
  bool keyPress(KeyPress key) override;
  bool keyRelease(KeyPress key) override;
  bool mousePress(MouseKeyPress button) override;
  bool mouseRelease(MouseKeyPress button) override;
};

#endif
