#ifndef UDEN_RENDER
#define UDEN_RENDER

#include <memory>
#include <vector>
#include <iostream>
#include <sstream>
#include <iterator>
#include <cmath>
#include <limits>

#include <raylib.h>
#include <raymath.h>
#include <json.hpp>

#include "Colorscheme.h"
#include "ShaderStore.h"
#include "Ui.h"

using json = nlohmann::json;

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

  // Graphics
  void draw();
  float extent();
  float distanceToPoint(Vector2 pos);

  // Area/Boundary management
  bool canCollapse();
  void collapse(std::shared_ptr<Area> toDelete);
  void deleteArea(Area* toDelete);
  void moveBoundary(Vector2 screenPos);

  // Serialization
  json serialize();

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

enum RenderAnchor {
  LEFT,
  CENTER,
  RIGHT,
};

enum AreaType {
  EMPTY,
  VIEWPORT3D,
};

class Area {
public:
  Area(
    int screenW,
    int screenH,
    Rectangle screenRect,
    Vector2 screenPos,
    int id,
    AreaType type,
    std::shared_ptr<Colorscheme> colorscheme,
    std::shared_ptr<ShaderStore> shaderStore
  );
  ~Area();

  // Graphics
  void draw();
  bool containsPoint(Vector2 localPos);
  void receiveMousePos(Vector2 mousePos);
  void receiveMouseDown(Vector2 mousePos);
  void receiveMouseUp(Vector2 mousePos);
  void updateShaders();

  // Event Listeners
  void setOnMouseEnter(std::function<void(Ui*)>);
  void setOnMouseExit(std::function<void(Ui*)>);

  // Area/Boundary management
  void addUi(std::shared_ptr<Ui>& ui);
  void dumpInfo();
  void deleteBoundary(Boundary* toDelete);
  void deleteThisFromBoundaries();
  bool isLeftOf(Boundary* bdry);
  bool isRightOf(Boundary* bdry);
  bool isBelow(Boundary* bdry);
  bool isAbove(Boundary* bdry);

  // Serialization
  json serialize();

  Rectangle screenRect;
  Vector2 screenPos;

  RenderTexture paneTexture;
  int id;
  std::shared_ptr<Boundary> leftBdry;
  std::shared_ptr<Boundary> rightBdry;
  std::shared_ptr<Boundary> upBdry;
  std::shared_ptr<Boundary> downBdry;
  bool active = true;
  RenderAnchor anchor = LEFT;
  AreaType type;

  static const int minimumExtent = 10;

private:
  // Internal constructors for different types of areas
  void buildViewport3D();
  void buildEmpty();
  void buildTypeDropDown();
  void resetType();

  std::shared_ptr<Colorscheme> colorscheme;
  std::shared_ptr<ShaderStore> shaderStore;

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
  void collapseBoundary(Vector2 mousePos);
  void dumpPanes();

  // Serialization
  json serialize();
  void deserialize(json serialized);

  std::vector<std::shared_ptr<Area>> areas;

  std::vector<std::shared_ptr<Boundary>> boundaries;

private:
  int screenW;
  int screenH;
  int nextPaneId = 1;
  int nextBdryId = 1;

  float mouseBoundaryTolerance = 15.f;

  std::shared_ptr<Colorscheme> colorscheme;
  std::shared_ptr<ShaderStore> shaderStore;
  Font font;

  std::shared_ptr<Boundary> grabbed = nullptr;

  std::shared_ptr<Boundary> findBoundary(Vector2 pos, float radius);
  std::shared_ptr<Boundary> findBoundary(int id);
  void deleteBoundary(std::shared_ptr<Boundary> toDelete);

  std::shared_ptr<Area> findArea(Vector2 pos);
  std::shared_ptr<Area> findArea(int id);
  void deleteArea(std::shared_ptr<Area> toDelete);
};

#endif
