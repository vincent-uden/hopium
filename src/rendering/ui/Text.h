#ifndef UDEN_UI_TEXT
#define UDEN_UI_TEXT

#include <string>

#include "../Ui.h"

#include <raylib.h>

namespace Ui {

enum TextAlignment {
  LEFT,
  CENTER,
  RIGHT
};

class Text: public Ui {
public:
  Text();
  Text(std::string text);
  ~Text();

  void move(Vector2 distance) override;
  void setPos(Vector2 pos) override;
  void draw() override;
  void receiveMousePos(Vector2 mousePos) override;
  void receiveMouseDown(Vector2 mousePos) override;
  void receiveMouseUp(Vector2 mousePos) override;
  void receiveMouseWheel(Vector2 mousePos, float movement) override;
  Vector2 getSize() override;

  void setAlignment(TextAlignment align);
  void setColor(Color c);
  void setText(std::string text);

private:
  bool hovered = false;
  TextAlignment align = TextAlignment::LEFT;

  Vector2 pos;
  Vector2 size;

  std::string text;
  Color color;
};

}

#endif
