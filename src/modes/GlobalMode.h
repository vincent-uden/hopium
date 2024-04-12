#ifndef UDEN_GLOBAL_MODE
#define UDEN_GLOBAL_MODE

#include "../Event.h"
#include "../Mode.h"
#include "../ApplicationState.h"

class GlobalMode : public Mode {
public:
  GlobalMode();
  ~GlobalMode();

  bool processEvent(AppEvent event) override;
  bool keyPress(KeyPress key) override;
  bool keyRelease(KeyPress key) override;
  bool mousePress(MouseKeyPress button) override;
  bool mouseRelease(MouseKeyPress button) override;
};


#endif
