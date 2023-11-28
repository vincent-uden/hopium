#include "OcctScene.h"

OcctScene::OcctScene() {
}

OcctScene::~OcctScene() {
}

void OcctScene::createPoint(double x, double y, double z) {
  points.push_back(gp_Pnt(x, y, z));
  ids.push_back(nextId++);
}

std::vector<std::shared_ptr<RasterVertex>> OcctScene::rasterizePoints() {
  std::vector<std::shared_ptr<RasterVertex>> rasterPoints;
  for (int i = 0; i < points.size(); i++) {
    rasterPoints.push_back(std::shared_ptr<RasterVertex>(new RasterVertex(points[i].X(), points[i].Y(), points[i].Z())));
    rasterPoints[i]->id = ids[i];
  }
  return rasterPoints;
}
