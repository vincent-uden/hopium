#ifndef UDEN_OCCT_SCENE
#define UDEN_OCCT_SCENE

#include <memory>
#include <vector>

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <GC_MakeSegment.hxx>
#include <gp_Pnt.hxx>
#include <TopoDS_Shape.hxx>
#include <Standard_Handle.hxx>

#include "raylib.h"

#include "Scene.h"

class OcctScene {
public:
  OcctScene();
  ~OcctScene();

  void createPoint(double x, double y, double z);
  void createLine(gp_Pnt p1, gp_Pnt p2);

  // This API is mega-subject to change.
  std::vector<std::shared_ptr<RasterVertex>> rasterizePoints();
  std::vector<std::shared_ptr<RasterShape>> rasterizeShapes();

private:
  std::vector<gp_Pnt> points;
  std::vector<size_t> ids;

  std::shared_ptr<RasterShape> createRasterShape(const TopoDS_Edge& shape);
  std::shared_ptr<RasterShape> createRasterShape(const TopoDS_Shape& shape);

  size_t nextId = 0;
  // Should probably have a vector of TopoDS_Shape objects

  std::vector<TopoDS_Shape> shapes;
};

#endif
