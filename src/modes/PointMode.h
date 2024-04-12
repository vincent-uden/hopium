#ifndef UDEN_POINT_MODE
#define UDEN_POINT_MODE

#include "../Event.h"
#include "../Mode.h"
#include "../ApplicationState.h"

class PointMode : public Mode {
public:
  PointMode();
  ~PointMode();

  bool processEvent(AppEvent event) override;
  bool keyPress(KeyPress key) override;
  bool keyRelease(KeyPress key) override;
  bool mousePress(MouseKeyPress button) override;
  bool mouseRelease(MouseKeyPress button) override;
};

#endif
