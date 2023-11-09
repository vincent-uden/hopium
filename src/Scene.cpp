#include "Scene.h"

std::vector<Texture2D> Scene::standardModelTextures = std::vector<Texture2D>();
Shader Scene::standardModelShader = (Shader) {0};

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

    standardModelShader = LoadShader("../shaders/vs.glsl", "../shaders/fs.glsl");
    int ambientLoc = GetShaderLocation(standardModelShader, "ambientColor");
    float ambColor[3] = { 1.0f, 1.0f, 1.0f };
    SetShaderValue(standardModelShader, ambientLoc, ambColor, SHADER_UNIFORM_VEC3);
  }
}

Scene::~Scene() {
}

void Scene::addBody(std::shared_ptr<RasterBody> body) {
  bodies.push_back(body);
}

void Scene::addBodyFromFile(std::string path) {
  std::shared_ptr<RasterBody> body(new RasterBody());
  body->loadFromFile(path);
  body->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = standardModelTextures[bodies.size() % standardModelTextures.size()];
  body->model.materials[0].shader = standardModelShader;
  addBody(body);
}

size_t Scene::nBodies() {
  return bodies.size();
}

std::shared_ptr<RasterBody> Scene::getBody(size_t i) {
  return bodies.at(i);
}
