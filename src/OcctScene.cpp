#include "OcctScene.h"

OcctScene::OcctScene() {
}

OcctScene::~OcctScene() {
}

void OcctScene::createPoint(double x, double y, double z) {
  points.push_back(gp_Pnt(x, y, z));
}

std::vector<std::shared_ptr<RasterPoint>> OcctScene::rasterizePoints() {
  std::vector<std::shared_ptr<RasterPoint>> rasterPoints;
  for (int i = 0; i < points.size(); i++) {
    rasterPoints.push_back(std::shared_ptr<RasterPoint>(new RasterPoint(points[i].X(), points[i].Y(), points[i].Z())));
  }
  return rasterPoints;
}
