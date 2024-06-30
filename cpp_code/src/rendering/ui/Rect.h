#ifndef UDEN_UI_RECT
#define UDEN_UI_RECT

#include <string>

#include "../Ui.h"

#include <raylib.h>

namespace Ui {

class Rect: public Ui {
public:
  Rect();
  ~Rect();

  void move(Vector2 distance) override;
  void setPos(Vector2 pos) override;
  void draw() override;
  void receiveMousePos(Vector2 mousePos) override;
  void receiveMouseDown(Vector2 mousePos) override;
  void receiveMouseUp(Vector2 mousePos) override;
  void receiveMouseWheel(Vector2 mousePos, float movement) override;
  Vector2 getSize() override;

  void setColor(Color c);
  void setSize(Vector2 size);

  bool contains(Vector2 mousePos);
private:
  bool hovered = false;

  Rectangle bounds = { 0, 0, 0, 0 };
  Color bgColor = { 0, 0, 0, 0 };
};

}

#endif

