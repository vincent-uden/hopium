#include "Scene.h"

std::vector<Texture2D> Scene::standardModelTextures = std::vector<Texture2D>();

Color RasterShape::activeColor() {
  return GREEN;
}

Color RasterShape::passiveColor() {
  return YELLOW;
}

RasterBody::RasterBody() {
}

RasterBody::~RasterBody() {
  if (hasLoadedModel) {
    UnloadModel(model);
  }
}

void RasterBody::draw() {
  DrawModel(model, pos, 1.f, WHITE);
}

double RasterBody::distanceFromRay(const Ray ray) {
  // TODO: Finish
  return 0.0;
}

void RasterBody::loadFromFile(std::string path) {
  if (hasLoadedModel) {
    UnloadModel(model);
  }
  model = LoadModel(path.c_str());
  pos = { 0, 0, 0};
  hasLoadedModel = true;
}

RasterVertex::RasterVertex() {
  x = 0.0;
  y = 0.0;
  z = 0.0;
}

RasterVertex::RasterVertex(double x, double y, double z) {
  this->x = x;
  this->y = y;
  this->z = z;
}

RasterVertex::~RasterVertex() {
}

void RasterVertex::draw() {
  DrawSphere((Vector3) { (float) x, (float) y, (float) z }, 0.2, color);
}

double RasterVertex::distanceFromRay(const Ray ray) {
  Vector3 pos = (Vector3) { (float) x, (float) y, (float) z };
  Vector3 posMinusRayPos = Vector3Subtract(pos, ray.position);

  Vector3 intermediate = Vector3Subtract(
    posMinusRayPos,
    Vector3Scale(
      ray.direction,
      Vector3DotProduct(posMinusRayPos, ray.direction)
    )
  );

  return Vector3Length(intermediate);
}

RasterLine::RasterLine() {
}

RasterLine::RasterLine(Vector3 p1, Vector3 p2) {
  this->p1 = p1;
  this->p2 = p2;
}

RasterLine::~RasterLine() {
}

void RasterLine::draw() {
  DrawLine3D(p1, p2, color);
}

double RasterLine::distanceFromRay(const Ray ray) {
  // TODO
  return INFINITY;
}

RasterFace::RasterFace() {
}

RasterFace::RasterFace(std::vector<Vector3> vertices) {
  triangles = vertices;
  color.a = 150;
}

RasterFace::~RasterFace() {
}

void RasterFace::draw() {
  for (int i = 0; i < triangles.size(); i += 3) {
    DrawTriangle3D(triangles[i], triangles[i + 1], triangles[i + 2], color);
    DrawTriangle3D(triangles[i + 2], triangles[i + 1], triangles[i], color);
  }
}

double RasterFace::distanceFromRay(const Ray ray) {
  // TODO
  return INFINITY;
}

Color RasterFace::activeColor() {
  Color out = GREEN;
  out.a = 150;
  return out;
}

Color RasterFace::passiveColor() {
  Color out = YELLOW;
  out.a = 150;
  return out;
}

RasterSolid::RasterSolid() {
}

RasterSolid::RasterSolid(Mesh mesh) {
  this->mesh = mesh;
  color.a = 150;
}

RasterSolid::~RasterSolid() {
}

void RasterSolid::draw() {
  // TODO
}

double RasterSolid::distanceFromRay(const Ray ray) {
  // TODO
  return INFINITY;
}

RasterTodo::RasterTodo() {
}

RasterTodo::~RasterTodo() {
}

void RasterTodo::draw() {
}

double RasterTodo::distanceFromRay(const Ray ray) {
  return INFINITY;
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

void Scene::setPoints(std::vector<std::shared_ptr<RasterVertex>> points) {
  this->points.clear();
  this->points = points;
}

void Scene::setShapes(std::vector<std::shared_ptr<RasterShape>> shapes) {
  this->shapes.clear();
  this->shapes = shapes;
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

size_t Scene::nShapes() {
  return shapes.size();
}

std::shared_ptr<RasterBody> Scene::getBody(size_t i) {
  return bodies.at(i);
}

std::shared_ptr<RasterVertex> Scene::getPoint(size_t i) {
  return points.at(i);
}

std::shared_ptr<RasterShape> Scene::getShape(size_t i) {
  return shapes.at(i);
}

std::optional<std::shared_ptr<RasterVertex>> Scene::queryVertex(Ray ray, double selectionThreshold) {
  int closestPoint = -1;
  double closestDist = std::numeric_limits<double>::max();

  for (int i = 0; i < nPoints(); ++i) {
    double dist = getPoint(i)->distanceFromRay(ray) ;
    if (dist < selectionThreshold && dist < closestDist) {
      closestPoint = i;
      closestDist = dist;
    }
  }

  if (closestPoint == -1) {
    return std::nullopt;
  } else {
    return std::optional<std::shared_ptr<RasterVertex>>(getPoint(closestPoint));
  }
}
