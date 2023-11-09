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
  void addBodyFromFile(std::string path);
  size_t nBodies();
  std::shared_ptr<RasterBody> getBody(size_t i);

  static Shader standardModelShader;

private:
  std::vector<std::shared_ptr<RasterBody>> bodies;

  static std::vector<Texture2D> standardModelTextures;
};
#endif
