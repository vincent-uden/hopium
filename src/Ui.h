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
#include "ConstraintGraph.h"
#include "Event.h"
#include "Scene.h"

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

class Rect: public Ui {
public:
  Rect();
  ~Rect();

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

class Viewport: public Ui {
public:
  Viewport();
  ~Viewport();

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
  Vector2 getSize() override;

  Vector2 btnSize;

private:
  Vector2 pos;
  int margin;

  std::vector<std::string> btnNames;
  std::vector<std::shared_ptr<Text>> btnLbls;
  std::vector<std::shared_ptr<Rect>> btnBgs;
};

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
  Vector2 getSize() override;

  void setImgPath(std::string path);
  void setHoverTooltip(std::string tooltip);

private:
  Texture2D texture;
  bool loadedTexture;
  bool hovered;
  std::chrono::time_point<std::chrono::system_clock> hoverBegin;
  std::chrono::duration<double> tooltipDelay;

  std::shared_ptr<Text> hoverTooltip;
  std::shared_ptr<Rect> hoverTooltipBg;
  std::shared_ptr<Rect> bg;

  Vector2 pos;
  Vector2 size;
};

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
  Vector2 getSize() override;

  void addChild(std::shared_ptr<Ui> child);

private:
  std::vector<std::shared_ptr<Ui>> children;
  int margin = 4;

  Vector2 pos;
  Vector2 size;
  bool hovered;
};

class GraphViewer: public Ui {
public:
  GraphViewer();
  ~GraphViewer();

  void move(Vector2 distance) override;
  void setPos(Vector2 pos) override;
  void draw() override;
  void receiveMousePos(Vector2 mousePos) override;
  void receiveMouseDown(Vector2 mousePos) override;
  void receiveMouseUp(Vector2 mousePos) override;
  Vector2 getSize() override;

  void setAreaPointers(Rectangle* screenRect, Vector2* screenPos, RenderTexture* texture);
  void setGraph(std::shared_ptr<ConstraintGraph> graph);

private:
  Vector2 toScreenSpace(const Vector2 p);
  Vector2 toGraphSpace(const Vector2 p);

  Vector2 lastMousePos;
  Vector2 panOffset;
  Vector2 pos;
  float centralAttraction = 0.4f;
  float dt = 0.01;
  float edgeThickness = 2.f;
  float pullForce = 20.f;
  float pushForce = 0.3f;
  float scale = 200.f;
  float springLength = 0.2f;
  float selectThreshold = 15.0f;
  float velocityDamping = 0.98;
  int grabbedId = -1;
  int hoveredId = -1;
  std::shared_ptr<ConstraintGraph> graph;
  std::vector<Vector2> nodeAcc;
  std::vector<Vector2> nodePos;
  std::vector<Vector2> nodeVel;

  Rectangle* areaScreenRect = nullptr;
  Vector2* areaScreenPos = nullptr;
  RenderTexture* areaTexture = nullptr;
};

}

#endif
