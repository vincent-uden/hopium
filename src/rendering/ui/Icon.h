#ifndef UDEN_UI_ICON
#define UDEN_UI_ICON

#include <chrono>
#include <string>

#include "../../ApplicationState.h"
#include "../Ui.h"
#include "Rect.h"
#include "Text.h"

#include <raylib.h>

namespace Ui {

class Icon: public Ui {
public:
  Icon();
  ~Icon();

  void move(Vector2 distance) override;
  void setPos(Vector2 pos) override;
  void draw() override;
  void receiveMousePos(Vector2 mousePos) override;
  void receiveMouseDown(Vector2 mousePos) override;
  void receiveMouseUp(Vector2 mousePos) override;
  void receiveMouseWheel(Vector2 mousePos, float movement) override;
  Vector2 getSize() override;

  void setImgPath(std::string path);
  void setHoverTooltip(std::string tooltip);
  void setBgColor(Color color);

  std::string tooltip;
  Texture2D texture;
  bool loadedTexture;

private:
  bool hovered;
  std::chrono::time_point<std::chrono::system_clock> hoverBegin;
  std::chrono::duration<double> tooltipDelay;

  std::shared_ptr<Text> hoverTooltip;
  std::shared_ptr<Rect> hoverTooltipBg;
  std::shared_ptr<Rect> bg;

  Vector2 pos;
  Vector2 size;
};

}

#endif
