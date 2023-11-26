#ifndef UDEN_SCENE
#define UDEN_SCENE

#include <memory>
#include <string>
#include <vector>
#include <raylib.h>

#include "ShaderStore.h"

class RasterBody {
public:
  RasterBody();
  ~RasterBody();

  void loadFromFile(std::string path);

  Model model;
  Vector3 pos;
private:
  bool hasLoadedModel = false;
};

class RasterPoint {
public:
  RasterPoint();
  RasterPoint(double x, double y, double z);

  ~RasterPoint();

  double x;
  double y;
  double z;
};

class Scene {
public:
  Scene(std::shared_ptr<ShaderStore> shaderStore);
  ~Scene();

  void addBody(std::shared_ptr<RasterBody> body);
  void addBodyFromFile(std::string path);
  void setPoints(std::vector<std::shared_ptr<RasterPoint>> points);

  size_t nBodies();
  size_t nPoints();
  std::shared_ptr<RasterBody> getBody(size_t i);
  std::shared_ptr<RasterPoint> getPoint(size_t i);

private:
  std::vector<std::shared_ptr<RasterBody>> bodies;
  std::vector<std::shared_ptr<RasterPoint>> points;

  static std::vector<Texture2D> standardModelTextures;

  std::shared_ptr<ShaderStore> shaderStore;
};
#endif
