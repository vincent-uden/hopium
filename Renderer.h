#ifndef UDEN_RENDER
#define UDEN_RENDER

#include <memory>
#include <vector>
#include <iostream>
#include <sstream>
#include <iterator>

#include <raylib.h>
#include <raymath.h>

#include "Ui.h"
#include "Colorscheme.h"

// Forward declarations
class Area;

enum BoundaryOrientation {
  VERTICAL,
  HORIZONTAL
};

class Boundary {
public:
  Boundary(std::shared_ptr<Colorscheme> colorscheme);
  ~Boundary();

  bool canCollapse();
  void collapse(std::shared_ptr<Area> toDelete);
  void deleteArea(Area* toDelete);
  void draw();
  void moveBoundary(Vector2 screenPos);
  float extent();
  float distanceToPoint(Vector2 pos);

  BoundaryOrientation orientation;
  bool active = true;
  int id;
  int thickness = 3;

  // Left / Up
  std::vector<std::shared_ptr<Area>> side1;
  // Right / Down
  std::vector<std::shared_ptr<Area>> side2;
private:
  std::shared_ptr<Colorscheme> colorscheme;
};

class Area {
public:
  Area(int screenW, int screenH, Rectangle screenRect, Vector2 screenPos, int id, std::shared_ptr<Colorscheme> colorscheme);
  ~Area();

  // Graphics
  void draw();
  bool containsPoint(Vector2 localPos);
  void receiveMousePos(Vector2 mousePos);

  // Event Listeners
  void setOnMouseEnter(std::function<void(Ui*)>);
  void setOnMouseExit(std::function<void(Ui*)>);

  // Area/Boundary management
  void addUi(std::shared_ptr<Ui>& ui);
  void dumpInfo();
  void deleteBoundary(Boundary* toDelete);
  void deleteThisFromBoundaries();

  Rectangle screenRect;
  Vector2 screenPos;

  RenderTexture paneTexture;
  int paneId;
  std::shared_ptr<Boundary> leftBdry;
  std::shared_ptr<Boundary> rightBdry;
  std::shared_ptr<Boundary> upBdry;
  std::shared_ptr<Boundary> downBdry;
  bool active = true;

  static const int minimumExtent = 10;

private:
  std::shared_ptr<Colorscheme> colorscheme;

  std::vector<std::shared_ptr<Ui>> contents;

  bool hovered = false;
  std::function<void(Area*)> onMouseEnter = nullptr;
  std::function<void(Area*)> onMouseExit = nullptr;
};

class Renderer {
public:
  Renderer(int screenW, int screenH);
  ~Renderer();

  void draw();
  void receiveMousePos(Vector2 mousePos);
  void mouseDown(Vector2 mousePos);
  void mouseUp(Vector2 mousePos);
  void splitPaneHorizontal(Vector2 mousePos);
  void splitPaneVertical(Vector2 mousePos);
  void dumpPanes();

  std::vector<std::shared_ptr<Area>> areas;
  std::vector<std::shared_ptr<Boundary>> boundaries;
private:
  int screenW;
  int screenH;
  int nextPaneId = 1;
  int nextBdryId = 1;

  float mouseBoundaryTolerance = 15.f;

  std::shared_ptr<Colorscheme> colorscheme;
  Font font;

  std::shared_ptr<Boundary> grabbed = nullptr;

  std::shared_ptr<Boundary> findBoundary(Vector2 pos, float radius);

  std::shared_ptr<Area> findPane(Vector2 pos);
};

#endif
