#ifndef UDEN_UI
#define UDEN_UI

#include <memory>
#include <raylib.h>
#include <string>
#include <vector>
#include <functional>
#include <cmath>
#include <cfloat>
#include <iostream>

#include "Event.h"
#include "Colorscheme.h"
#include "Scene.h"

class Ui {
public:
  virtual void move(Vector2 distance)=0;
  virtual void setPos(Vector2 pos)=0;
  virtual void draw()=0;
  virtual void receiveMousePos(Vector2 mousePos)=0;
  virtual void receiveMouseDown(Vector2 mousePos)=0;
  virtual void receiveMouseUp(Vector2 mousePos)=0;

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

  void move(Vector2 distance) override;
  void setPos(Vector2 pos) override;
  void draw() override;
  void receiveMousePos(Vector2 mousePos) override;
  void receiveMouseDown(Vector2 mousePos) override;
  void receiveMouseUp(Vector2 mousePos) override;

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

  void move(Vector2 distance) override;
  void setPos(Vector2 pos) override;
  void draw() override;
  void receiveMousePos(Vector2 mousePos) override;
  void receiveMouseDown(Vector2 mousePos) override;
  void receiveMouseUp(Vector2 mousePos) override;

  void setColor(Color c);
  void setSize(Vector2 size);

  bool contains(Vector2 mousePos);
private:
  bool hovered = false;

  Rectangle bounds = { 0, 0, 0, 0 };
  Color bgColor = { 0, 0, 0, 0 };
};

class UiDropDown: public Ui {
public:
  UiDropDown(std::string label, std::vector<std::string> options);
  ~UiDropDown();

  void move(Vector2 distance) override;
  void setPos(Vector2 pos) override;
  void draw() override;
  void receiveMousePos(Vector2 mousePos) override;
  void receiveMouseDown(Vector2 mousePos) override;
  void receiveMouseUp(Vector2 mousePos) override;

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
  UiText label;
  UiRect labelBg;
  std::vector<std::shared_ptr<UiText>> uiOptions;
  std::vector<std::shared_ptr<UiRect>> uiOptionsBg;
  int padding = 4;

  // State
  std::vector<std::string> options;
  std::function<void(std::string)> onSelected;
  bool hovered = false;
  bool open = false;
};

class Ui3DViewport: public Ui {
public:
  Ui3DViewport();
  ~Ui3DViewport();

  void move(Vector2 distance) override;
  void setPos(Vector2 pos) override;
  void draw() override;
  void receiveMousePos(Vector2 mousePos) override;
  void receiveMouseDown(Vector2 mousePos) override;
  void receiveMouseUp(Vector2 mousePos) override;

  void setScene(std::shared_ptr<Scene> scene);

  Camera3D camera = { 0 };
private:

  std::shared_ptr<Scene> scene;

  // Ground Quad
  Vector3 g0 = { -50.0f, 0.0f, -50.0f };
  Vector3 g1 = { -50.0f, 0.0f,  50.0f };
  Vector3 g2 = {  50.0f, 0.0f,  50.0f };
  Vector3 g3 = {  50.0f, 0.0f, -50.0f };
};

class UiToolList: public Ui {
public:
  UiToolList();
  ~UiToolList();

  void move(Vector2 distance) override;
  void setPos(Vector2 pos) override;
  void draw() override;
  void receiveMousePos(Vector2 mousePos) override;
  void receiveMouseDown(Vector2 mousePos) override;
  void receiveMouseUp(Vector2 mousePos) override;

  Vector2 btnSize;

private:
  Vector2 pos;
  int margin;

  std::vector<std::string> btnNames;
  std::vector<std::shared_ptr<UiText>> btnLbls;
  std::vector<std::shared_ptr<UiRect>> btnBgs;
};

#endif
