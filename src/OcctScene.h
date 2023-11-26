#ifndef UDEN_OCCT_SCENE
#define UDEN_OCCT_SCENE

#include <memory>
#include <vector>

#include <gp_Pnt.hxx>

#include "raylib.h"

#include "Scene.h"

class OcctScene {
public:
  OcctScene();
  ~OcctScene();

  void createPoint(double x, double y, double z);

  // This API is mega-subject to change.
  std::vector<std::shared_ptr<RasterPoint>> rasterizePoints();

private:
  std::vector<gp_Pnt> points;
};

#endif
