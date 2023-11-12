#ifndef UDEN_COLORSCHEME
#define UDEN_COLORSCHEME

#include <raylib.h>

class Colorscheme {
public:
  Colorscheme(Font* font);
  ~Colorscheme();

  // Scheme names taken from Material Design
  // https://m2.material.io/design/color/the-color-system.html#color-theme-creation
  Color primary          = {0, 0, 0, 255};
  Color primaryVariant   = {0, 0, 0, 255};
  Color secondary        = {60, 60, 60, 255};
  Color secondaryVariant = {70, 70, 70, 255};

  Color background = {51, 51, 51, 255};
  Color surface    = {30, 30, 30, 255};
  Color error      = {0, 0, 0, 255};

  Color onPrimary    = {0, 0, 0, 255};
  Color onSecondary  = {0, 0, 0, 255};
  Color onBackground = {255, 255, 255, 255};
  Color onSurface    = {255, 255, 255, 255};
  Color onError      = {0, 0, 0, 255};
  // ---

  Color borderColor = {31, 31, 31, 255};

  Font font;
};
#endif
