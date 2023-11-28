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
  std::vector<std::shared_ptr<RasterVertex>> rasterizePoints();

private:
  std::vector<gp_Pnt> points;
  std::vector<size_t> ids;

  size_t nextId = 0;
  // Should probably have a vector of TopoDS_Shape objects
};

#endif
