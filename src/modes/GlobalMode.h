#ifndef UDEN_GLOBAL_MODE
#define UDEN_GLOBAL_MODE

#include "../Event.h"
#include "../Mode.h"

class GlobalMode : public Mode {
public:
  GlobalMode();
  ~GlobalMode();

  bool keyPress(KeyPress key) override;
  bool keyRelease(KeyPress key) override;
  bool mousePress(MouseKeyPress button) override;
  bool mouseRelease(MouseKeyPress button) override;
};


#endif
