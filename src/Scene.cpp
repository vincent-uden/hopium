#include "Scene.h"

std::vector<Texture2D> Scene::standardModelTextures = std::vector<Texture2D>();

RasterBody::RasterBody() {
}

RasterBody::~RasterBody() {
  if (hasLoadedModel) {
    UnloadModel(model);
  }
}

void RasterBody::loadFromFile(std::string path) {
  if (hasLoadedModel) {
    UnloadModel(model);
  }
  model = LoadModel(path.c_str());
  pos = { 0, 0, 0};
  hasLoadedModel = true;
}

RasterPoint::RasterPoint() {
  x = 0.0;
  y = 0.0;
  z = 0.0;
}

RasterPoint::RasterPoint(double x, double y, double z) {
  this->x = x;
  this->y = y;
  this->z = z;
}

RasterPoint::~RasterPoint() {
}

Scene::Scene(std::shared_ptr<ShaderStore> shaderStore) {
  this->shaderStore = shaderStore;
  if (standardModelTextures.size() == 0) {
    Color colors[7] = {
      {255, 179, 186, 255},
      {255, 223, 186, 255},
      {255, 255, 186, 255},
      {186, 255, 201, 255},
      {186, 255, 255, 255},
      {201, 201, 255, 255},
      {241, 203, 255, 255},
    };

    Image img = GenImageColor(10, 10, RED);
    for (Color& c : colors) {
      ImageClearBackground(&img, c);
      Texture2D texture = LoadTextureFromImage(img);
      standardModelTextures.push_back(texture);
    }
  }
}

Scene::~Scene() {
}

void Scene::addBody(std::shared_ptr<RasterBody> body) {
  bodies.push_back(body);
}

void Scene::setPoints(std::vector<std::shared_ptr<RasterPoint>> points) {
  this->points.clear();
  this->points = points;
}

void Scene::addBodyFromFile(std::string path) {
  std::shared_ptr<RasterBody> body(new RasterBody());
  body->loadFromFile(path);
  body->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = standardModelTextures[bodies.size() % standardModelTextures.size()];
  body->model.materials[0].shader = shaderStore->standardModelShader;
  addBody(body);
}

size_t Scene::nBodies() {
  return bodies.size();
}

size_t Scene::nPoints() {
  return points.size();
}

std::shared_ptr<RasterBody> Scene::getBody(size_t i) {
  return bodies.at(i);
}

std::shared_ptr<RasterPoint> Scene::getPoint(size_t i) {
  return points.at(i);
}
