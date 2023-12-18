#ifndef UDEN_UI
#define UDEN_UI

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <cmath>
#include <cfloat>
#include <iostream>

#include "ApplicationState.h"
#include "Colorscheme.h"
#include "Event.h"
#include "Scene.h"

#include <raylib.h>
#include <raymath.h>

class Ui {
public:
  virtual void move(Vector2 distance)=0;
  virtual void setPos(Vector2 pos)=0;
  virtual void draw()=0;
  virtual void receiveMousePos(Vector2 mousePos)=0;
  virtual void receiveMouseDown(Vector2 mousePos)=0;
  virtual void receiveMouseUp(Vector2 mousePos)=0;
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
  Vector2 getSize() override;

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
  Vector2 getSize() override;

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
  Vector2 getSize() override;

  void setScene(std::shared_ptr<Scene> scene);
  void setAreaPointers(Rectangle* screenRect, Vector2* screenPos, RenderTexture* texture);

  Camera3D camera = { 0 };
private:

  Ray getNonOffsetMouseRay(Vector2 mousePos);

  std::shared_ptr<Scene> scene;

  Rectangle* areaSreenRect = nullptr;
  Vector2* areaScreenPos = nullptr;
  RenderTexture* areaTexture = nullptr;

  // Ground Quad
  Vector3 g0 = { -20.0f, 0.0f, -20.0f };
  Vector3 g1 = { -20.0f, 0.0f,  20.0f };
  Vector3 g2 = {  20.0f, 0.0f,  20.0f };
  Vector3 g3 = {  20.0f, 0.0f, -20.0f };

  Vector2 lastMousePos = { 0, 0 };
  double cameraSensetivity = 0.01;
  double cameraRadius = 34.64;

  Ray lastRay = { 0 };
  int hoveredId = -1;
  float lastDist = FLT_MAX;
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
  Vector2 getSize() override;

  Vector2 btnSize;

private:
  Vector2 pos;
  int margin;

  std::vector<std::string> btnNames;
  std::vector<std::shared_ptr<UiText>> btnLbls;
  std::vector<std::shared_ptr<UiRect>> btnBgs;
};

class UiIcon: public Ui {
public:
  UiIcon();
  ~UiIcon();

  void move(Vector2 distance) override;
  void setPos(Vector2 pos) override;
  void draw() override;
  void receiveMousePos(Vector2 mousePos) override;
  void receiveMouseDown(Vector2 mousePos) override;
  void receiveMouseUp(Vector2 mousePos) override;
  Vector2 getSize() override;

  void setImgPath(std::string path);
  void setHoverTooltip(std::string tooltip);

private:
  Texture2D texture;
  bool loadedTexture;
  bool hovered;
  std::chrono::time_point<std::chrono::system_clock> hoverBegin;
  std::chrono::duration<double> tooltipDelay;

  std::shared_ptr<UiText> hoverTooltip;
  std::shared_ptr<UiRect> hoverTooltipBg;
  std::shared_ptr<UiRect> bg;

  Vector2 pos;
  Vector2 size;
};

class UiRow: public Ui {
public:
  UiRow();
  ~UiRow();

  void move(Vector2 distance) override;
  void setPos(Vector2 pos) override;
  void draw() override;
  void receiveMousePos(Vector2 mousePos) override;
  void receiveMouseDown(Vector2 mousePos) override;
  void receiveMouseUp(Vector2 mousePos) override;
  Vector2 getSize() override;

  void addChild(std::shared_ptr<Ui> child);

private:
  std::vector<std::shared_ptr<Ui>> children;
  int margin = 4;

  Vector2 pos;
  Vector2 size;
  bool hovered;
};

#endif
