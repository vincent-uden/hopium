#ifndef UDEN_UI_TOOLLIST
#define UDEN_UI_TOOLLIST

#include <string>

#include  "../../ApplicationState.h"
#include "../Ui.h"
#include "Rect.h"
#include "Text.h"

#include <raylib.h>

namespace Ui {

class ToolList: public Ui {
public:
  ToolList();
  ~ToolList();

  void move(Vector2 distance) override;
  void setPos(Vector2 pos) override;
  void draw() override;
  void receiveMousePos(Vector2 mousePos) override;
  void receiveMouseDown(Vector2 mousePos) override;
  void receiveMouseUp(Vector2 mousePos) override;
  void receiveMouseWheel(Vector2 mousePos, float movement) override;
  Vector2 getSize() override;

  Vector2 btnSize;

private:
  Vector2 pos;
  int margin;

  std::vector<std::string> btnNames;
  std::vector<std::shared_ptr<Text>> btnLbls;
  std::vector<std::shared_ptr<Rect>> btnBgs;
};

}

#endif
