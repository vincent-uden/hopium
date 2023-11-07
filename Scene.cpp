#include "Scene.h"
#include "raylib.h"

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

Scene::Scene() {
}

Scene::~Scene() {
}

void Scene::addBody(std::shared_ptr<RasterBody> body) {
  bodies.push_back(body);
}

size_t Scene::nBodies() {
  return bodies.size();
}

std::shared_ptr<RasterBody> Scene::getBody(size_t i) {
  return bodies.at(i);
}
