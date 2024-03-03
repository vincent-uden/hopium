#ifndef UDEN_UI_DROPDOWN
#define UDEN_UI_DROPDOWN

#include <string>

#include "../Ui.h"
#include "Text.h"
#include "Rect.h"

#include <raylib.h>

namespace Ui {

class DropDown: public Ui {
public:
  DropDown(std::string label, std::vector<std::string> options);
  ~DropDown();

  void move(Vector2 distance) override;
  void setPos(Vector2 pos) override;
  void draw() override;
  void receiveMousePos(Vector2 mousePos) override;
  void receiveMouseDown(Vector2 mousePos) override;
  void receiveMouseUp(Vector2 mousePos) override;
  void receiveMouseWheel(Vector2 mousePos, float movement) override;
  Vector2 getSize() override;

  void setOnSelected(std::function<void(std::string)> f);

private:
  void select(size_t i);

  // Click boxes
  Vector2 pos;
  Vector2 btnSize;
  Vector2 listSize;

  // Ui components
  Color bgColor;
  Color fgColor;
  Text label;
  Rect labelBg;
  std::vector<std::shared_ptr<Text>> uiOptions;
  std::vector<std::shared_ptr<Rect>> uiOptionsBg;
  int padding = 4;

  // State
  std::vector<std::string> options;
  std::function<void(std::string)> onSelected;
  bool hovered = false;
  bool open = false;
};

}

#endif
