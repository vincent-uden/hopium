#ifndef UDEN_UI_ROW
#define UDEN_UI_ROW

#include <string>

#include "../Ui.h"

#include <raylib.h>

namespace Ui {
class Row: public Ui {
public:
  Row();
  ~Row();

  void move(Vector2 distance) override;
  void setPos(Vector2 pos) override;
  void draw() override;
  void receiveMousePos(Vector2 mousePos) override;
  void receiveMouseDown(Vector2 mousePos) override;
  void receiveMouseUp(Vector2 mousePos) override;
  void receiveMouseWheel(Vector2 mousePos, float movement) override;
  Vector2 getSize() override;

  void addChild(std::shared_ptr<Ui> child);

private:
  std::vector<std::shared_ptr<Ui>> children;
  int margin = 4;

  Vector2 pos;
  Vector2 size;
  bool hovered;
};

}

#endif
