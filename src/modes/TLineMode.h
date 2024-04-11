#ifndef UDEN_TLINE_MODE
#define UDEN_TLINE_MODE

#include "../Event.h"
#include "../Mode.h"

class TLineMode : public Mode {
public:
  TLineMode();
  ~TLineMode();

  bool keyPress(KeyPress key) override;
  bool keyRelease(KeyPress key) override;
  bool mousePress(MouseKeyPress button) override;
  bool mouseRelease(MouseKeyPress button) override;
};

#endif
