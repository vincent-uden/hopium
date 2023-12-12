#ifndef UDEN_OCCT_SCENE
#define UDEN_OCCT_SCENE

#include <memory>
#include <vector>

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRep_Tool.hxx>
#include <GC_MakeSegment.hxx>
#include <Standard_Handle.hxx>
#include <ShapeExtend_WireData.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <gp_Pnt.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <TopoDS_Face.hxx>
#include <ShapeFix_Wire.hxx>

#include "raylib.h"

#include "Scene.h"

class OcctScene {
public:
  OcctScene();
  ~OcctScene();

  void createPoint(double x, double y, double z);
  void createLine(gp_Pnt& p1, gp_Pnt& p2, double snapThreshold);
  void dumpShapes();

  // This API is mega-subject to change.
  std::vector<std::shared_ptr<RasterVertex>> rasterizePoints();
  std::vector<std::shared_ptr<RasterShape>> rasterizeShapes();
  std::pair<gp_Pnt, gp_Pnt> getEdgePoints(const TopoDS_Edge& edge);

private:
  std::vector<gp_Pnt> points;
  std::vector<size_t> ids;

  std::vector<std::shared_ptr<RasterShape>> createRasterShape(const TopoDS_Edge& shape);
  std::vector<std::shared_ptr<RasterShape>> createRasterShape(const TopoDS_Wire& shape);
  std::vector<std::shared_ptr<RasterShape>> createRasterShape(const TopoDS_Face& shape);
  std::vector<std::shared_ptr<RasterShape>> createRasterShape(const TopoDS_Shape& shape);

  std::string shapeType(const TopAbs_ShapeEnum& shapeType);

  bool isWireCyclic(const TopoDS_Wire& wire);

  size_t nextId = 0;
  // Should probably have a vector of TopoDS_Shape objects

  std::vector<TopoDS_Shape> shapes;
};

#endif
