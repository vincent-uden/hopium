#ifndef UDEN_SCENE
#define UDEN_SCENE

#include <memory>
#include <string>
#include <iostream>
#include <vector>

#include <raylib.h>
#include <raymath.h>

#include "ShaderStore.h"

class RasterShape {
public:
  virtual void draw() = 0;
  virtual double distanceFromRay(const Ray ray) = 0;

  size_t id = -1;
};

// This does not correspond to any TopoDS object from OCCT. It instead signifies
// an entire body which has been triangulated.
class RasterBody: public RasterShape {
public:
  RasterBody();
  ~RasterBody();

  void draw() override;
  double distanceFromRay(const Ray ray) override;

  void loadFromFile(std::string path);

  Model model;
  Vector3 pos;
private:
  bool hasLoadedModel = false;
};

class RasterVertex: public RasterShape {
public:
  RasterVertex();
  RasterVertex(double x, double y, double z);
  ~RasterVertex();

  void draw() override;
  double distanceFromRay(const Ray ray) override;

  double x;
  double y;
  double z;
  Color color = YELLOW;
};

class RasterLine: public RasterShape {
public:
  RasterLine();
  RasterLine(Vector3 p1, Vector3 p2);
  ~RasterLine();

  void draw() override;
  double distanceFromRay(const Ray ray) override;

  Vector3 p1;
  Vector3 p2;
  Color color = YELLOW;
};

class RasterTodo: public RasterShape {
public:
  RasterTodo();
  ~RasterTodo();

  void draw() override;
  double distanceFromRay(const Ray ray) override;
};

class Scene {
public:
  Scene(std::shared_ptr<ShaderStore> shaderStore);
  ~Scene();

  void addBody(std::shared_ptr<RasterBody> body);
  void addBodyFromFile(std::string path);
  void setPoints(std::vector<std::shared_ptr<RasterVertex>> points);
  void setShapes(std::vector<std::shared_ptr<RasterShape>> shapes);

  size_t nBodies();
  size_t nPoints();
  size_t nShapes();
  std::shared_ptr<RasterBody> getBody(size_t i);
  std::shared_ptr<RasterVertex> getPoint(size_t i);
  std::shared_ptr<RasterShape> getShape(size_t i);

private:
  std::vector<std::shared_ptr<RasterBody>> bodies;
  std::vector<std::shared_ptr<RasterVertex>> points;
  std::vector<std::shared_ptr<RasterShape>> shapes;

  static std::vector<Texture2D> standardModelTextures;

  std::shared_ptr<ShaderStore> shaderStore;
};
#endif
