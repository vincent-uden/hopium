#ifndef UDEN_OCCT_SCENE
#define UDEN_OCCT_SCENE

#include <memory>
#include <vector>

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <BRep_Tool.hxx>
#include <GC_MakeSegment.hxx>
#include <ShapeExtend_WireData.hxx>
#include <ShapeFix_Wire.hxx>
#include <Standard_Handle.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <gp_Pnt.hxx>

#include "Sketch.h"
#include "raylib.h"

#include "../rendering/Scene.h"


struct SketchData {
  std::shared_ptr<Sketch::NewSketch> sketch;
  Vector3 planeNormal;
  Vector3 planeX;
  Vector3 planeY;
  int id;
};

struct Matrix3 {
  float x11;
  float x12;
  float x13;
  float x21;
  float x22;
  float x23;
  float x31;
  float x32;
  float x33;
};

float determinant(const Matrix3& mat);
Vector3 solve(const Matrix3& A, const Vector3& b);

class ParametricScene {
public:
  ParametricScene();
  ~ParametricScene();

  // This API is mega-subject to change.
  std::vector<std::shared_ptr<RasterVertex>> rasterizePoints();
  std::vector<std::shared_ptr<RasterShape>> rasterizeShapes();

  void createPoint(int sketchId, Vector3 pos);
  void dumpShapes();

private:
  SketchData* findSketch(int id);

  Vector2 toSketchSpace(int sketchId, Vector3 worldPos);
  Vector3 toWorldSpace(Vector3 normal, Vector2 sketchPos);

  int nextSketchId = 0;
  int nextGeometryId = 0;
  std::vector<SketchData> sketches;
};

#endif
