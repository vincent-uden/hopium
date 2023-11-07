#ifndef UDEN_SCENE
#define UDEN_SCENE

#include <memory>
#include <string>
#include <vector>
#include <raylib.h>

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

class Scene {
public:
  Scene();
  ~Scene();

  void addBody(std::shared_ptr<RasterBody> body);
  size_t nBodies();
  std::shared_ptr<RasterBody> getBody(size_t i);

private:
  std::vector<std::shared_ptr<RasterBody>> bodies;
};

#endif
