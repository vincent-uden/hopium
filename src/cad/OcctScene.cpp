#include "OcctScene.h"
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <ShapeAnalysis_Wire.hxx>
#include <ShapeExtend_Status.hxx>
#include <TopoDS_Solid.hxx>
#include <gp_Pln.hxx>
#include <memory>

float determinant(const Matrix3& mat) {
  return (
      mat.x11*mat.x22*mat.x33 + mat.x12*mat.x23*mat.x31 + mat.x13*mat.x21*mat.x32
    ) - (
      mat.x13*mat.x22*mat.x31 + mat.x11*mat.x23*mat.x32 + mat.x12*mat.x21*mat.x33
  );
}

Vector3 solve(const Matrix3& A, const Vector3& b) {
  float detA = determinant(A);
  Vector3 out;
  Matrix3 tmp;

  tmp = A;
  tmp.x11 = b.x;
  tmp.x21 = b.y;
  tmp.x31 = b.z;
  out.x = determinant(tmp)/detA;
  tmp = A;
  tmp.x12 = b.x;
  tmp.x22 = b.y;
  tmp.x32 = b.z;
  out.y = determinant(tmp)/detA;
  tmp = A;
  tmp.x13 = b.x;
  tmp.x23 = b.y;
  tmp.x33 = b.z;
  out.z = determinant(tmp)/detA;

  return out;
}

ParametricScene::ParametricScene() {
  sketches.push_back({
      std::make_shared<Sketch::NewSketch>(),
      Vector3 { 0.0f, 0.0f, 1.0f },
      Vector3 { 1.0f, 0.0f, 0.0f },
      Vector3 { 0.0f, 1.0f, 0.0f },
      nextSketchId++,
  });
}

ParametricScene::~ParametricScene() {
}

std::vector<std::shared_ptr<RasterVertex>> ParametricScene::rasterizePoints() {
  std::vector<std::shared_ptr<RasterVertex>> out;

  for (auto& sketchData: sketches) {
    for (auto& p: sketchData.sketch->points) {
      std::shared_ptr<Sketch::Point> point = std::dynamic_pointer_cast<Sketch::Point>(p);
      out.push_back(std::make_shared<RasterVertex>(point->pos.x, point->pos.y, 0));
    }
  }

  return out;
}

std::vector<std::shared_ptr<RasterShape>> ParametricScene::rasterizeShapes() {
  std::vector<std::shared_ptr<RasterShape>> out;

  return out;
}

void ParametricScene::createPoint(int sketchId, Vector3 pos) {
  SketchData* data = findSketch(sketchId);
  if (data == nullptr) {
    return;
  }
  nextGeometryId++;
  auto point = std::make_shared<Sketch::Point>(
    std::make_shared<GeometricElement>(
      GeometricType::POINT,
      std::to_string(nextGeometryId)
    )
  );
  data->sketch->addPoint(point);
}

void ParametricScene::dumpShapes() {
  std::cout << "- Parametric Shapes -" << std::endl;
  for (auto& sketchData: sketches) {
    for (auto& p: sketchData.sketch->points) {
      std::cout << "Point: " << p->v->label << std::endl;
    }
  }
}

SketchData* ParametricScene::findSketch(int id) {
  for (auto& data: sketches) {
    if (data.id == id) {
      return &data;
    }
  }

  return nullptr;
}

Vector2 ParametricScene::toSketchSpace(int sketchId, Vector3 worldPos) {
  SketchData* data = findSketch(sketchId);
  if (data == nullptr) {
    return { 0, 0 };
  }

  Matrix3 sketchBasis = {
    data->planeX.x, data->planeX.y, data->planeX.z,
    data->planeY.x, data->planeY.y, data->planeY.z,
    data->planeNormal.x, data->planeNormal.y, data->planeNormal.z
  };

  Vector3 out = solve(sketchBasis, worldPos);
  return { out.x, out.y };
}

Vector3 ParametricScene::toWorldSpace(Vector3 normal, Vector2 sketchPos) {
  // Implement inverse transformation somehow
  return { 0, 0, 0 };
}

