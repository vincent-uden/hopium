#ifndef UDEN_UI_TEXT_INPUT
#define UDEN_UI_TEXT_INPUT

#include <string>

#include "./Text.h"
#include "../Ui.h"

#include <raylib.h>

namespace Ui {

class TextInput: public Ui {
public:
  TextInput();
  TextInput(std::string text);
  ~TextInput();

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
  void setCursor(int cursor);
  void setText(std::string text);

private:
  Vector2 baseDrawingPos();
  void calculateCursorLocation();

  bool hovered = false;
  TextAlignment align = TextAlignment::LEFT;

  Vector2 cursorStart;
  Vector2 cursorEnd;
  Vector2 pos;
  Vector2 size;

  int cursor = 0;

  std::string text;
  Color color;
};

}

#endif
