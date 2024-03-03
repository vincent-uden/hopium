#ifndef UDEN_UI
#define UDEN_UI

#include <functional>
#include <memory>

#include "Colorscheme.h"

#include <raylib.h>
#include <raymath.h>

namespace Ui {

class Ui {
public:
  virtual void move(Vector2 distance)=0;
  virtual void setPos(Vector2 pos)=0;
  virtual void draw()=0;
  virtual void receiveMousePos(Vector2 mousePos)=0;
  virtual void receiveMouseDown(Vector2 mousePos)=0;
  virtual void receiveMouseUp(Vector2 mousePos)=0;
  virtual void receiveMouseWheel(Vector2 mousePos, float movement)=0;
  virtual Vector2 getSize()=0;

  void setOnClick(std::function<void(Ui*)>);
  void setOnMouseEnter(std::function<void(Ui*)>);
  void setOnMouseExit(std::function<void(Ui*)>);
  static std::shared_ptr<Colorscheme> colorscheme;

protected:
  std::function<void(Ui*)> onClick;
  std::function<void(Ui*)> onMouseEnter;
  std::function<void(Ui*)> onMouseExit;
};

}

#endif
