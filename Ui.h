#ifndef UDEN_UI
#define UDEN_UI

#include <memory>
#include <raylib.h>
#include <string>
#include <vector>
#include <functional>

#include "Colorscheme.h"
#include "Scene.h"

class Ui {
public:
  virtual void move(Vector2 distance)=0;
  virtual void setPos(Vector2 pos)=0;
  virtual void draw()=0;
  virtual void receiveMousePos(Vector2 mousePos)=0;

  void setOnClick(std::function<void(Ui*)>);
  void setOnMouseEnter(std::function<void(Ui*)>);
  void setOnMouseExit(std::function<void(Ui*)>);
  static std::shared_ptr<Colorscheme> colorscheme;

protected:
  std::function<void(Ui*)> onClick;
  std::function<void(Ui*)> onMouseEnter;
  std::function<void(Ui*)> onMouseExit;

};

class UiText: public Ui {
public:
  UiText();
  UiText(std::string text);
  ~UiText();

  void move(Vector2 distance);
  void setPos(Vector2 pos);
  void draw();
  void receiveMousePos(Vector2 mousePos);

  void setColor(Color c);
  void setText(std::string text);

private:
  bool hovered = false;

  Vector2 pos;
  Vector2 size;

  std::string text;
  Color color;
};

class UiRect: public Ui {
public:
  UiRect();
  ~UiRect();

  void move(Vector2 distance);
  void setPos(Vector2 pos);
  void draw();
  void receiveMousePos(Vector2 mousePos);

  void setColor(Color c);
  void setSize(Vector2 size);
private:
  bool hovered = false;

  Rectangle bounds = { 0, 0, 0, 0 };
  Color bgColor = { 0, 0, 0, 0 };
};

class UiDropDown: public Ui {
public:
  UiDropDown(std::string label, std::vector<std::string> options);
  ~UiDropDown();

  void move(Vector2 distance);
  void setPos(Vector2 pos);
  void draw();
  void receiveMousePos(Vector2 mousePos);

  void setOnSelected(std::function<void(std::string)>);

private:
  Vector2 pos;
  Vector2 btnSize;
  Vector2 listSize;

  std::vector<std::string> options;
  Color bgColor;
  Color fgColor;

  UiText label;
  UiRect labelBg;
  std::vector<std::shared_ptr<UiText>> uiOptions;
  std::vector<std::shared_ptr<UiRect>> uiOptionsBg;
  bool hovered = false;
  bool open = false;
  int padding = 4;
};

class Ui3DViewport: public Ui {
public:
  Ui3DViewport();
  ~Ui3DViewport();

  void move(Vector2 distance);
  void setPos(Vector2 pos);
  void draw();
  void receiveMousePos(Vector2 mousePos);

  void setScene(std::shared_ptr<Scene> scene);

private:
  Camera3D camera = { 0 };

  std::shared_ptr<Scene> scene;
};

#endif
