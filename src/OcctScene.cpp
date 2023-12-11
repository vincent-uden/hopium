#include "OcctScene.h"
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRep_Tool.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>

OcctScene::OcctScene() {
}

OcctScene::~OcctScene() {
}

void OcctScene::createPoint(double x, double y, double z) {
  points.push_back(gp_Pnt(x, y, z));
  ids.push_back(nextId++);
}

void OcctScene::createLine(gp_Pnt& p1, gp_Pnt& p2, double snapThreshold) {
  gp_Pnt* start = &p1;
  gp_Pnt* end = &p2;
  for (gp_Pnt& p: points) {
    if (p1.Distance(p) < snapThreshold) {
      start = &p;
    }
    if (p2.Distance(p) < snapThreshold) {
      end = &p;
    }
  }
  Handle(Geom_TrimmedCurve) segment = GC_MakeSegment(*start, *end);
  TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(segment);
  shapes.push_back(edge);
}

std::vector<std::shared_ptr<RasterVertex>> OcctScene::rasterizePoints() {
  std::vector<std::shared_ptr<RasterVertex>> rasterPoints;
  for (int i = 0; i < points.size(); i++) {
    rasterPoints.push_back(std::shared_ptr<RasterVertex>(new RasterVertex(points[i].X(), points[i].Y(), points[i].Z())));
    rasterPoints[i]->id = ids[i];
  }
  return rasterPoints;
}

std::vector<std::shared_ptr<RasterShape>> OcctScene::rasterizeShapes() {
  std::vector<std::shared_ptr<RasterShape>> rasterShapes;
  for (const TopoDS_Shape &shape : shapes) {
    if (shape.ShapeType() == TopAbs_EDGE) {
      TopoDS_Edge edge = TopoDS::Edge(shape);
      rasterShapes.push_back(createRasterShape(edge));
    }
  }

  return rasterShapes;
}

std::shared_ptr<RasterShape> OcctScene::createRasterShape(const TopoDS_Edge &shape) {
  std::vector<gp_Pnt> points;
  for (TopExp_Explorer ex(shape, TopAbs_VERTEX); ex.More(); ex.Next()) {
    points.push_back(BRep_Tool::Pnt(TopoDS::Vertex(ex.Current())));
  }

  std::shared_ptr<RasterLine> out = std::shared_ptr<RasterLine>(new RasterLine(
        Vector3 {
          static_cast<float>(points[0].X()),
          static_cast<float>(points[0].Y()),
          static_cast<float>(points[0].Z())
        },
        Vector3 {
          static_cast<float>(points[1].X()),
          static_cast<float>(points[1].Y()),
          static_cast<float>(points[1].Z())
        }
  ));
  out->id = nextId++;
  return out;
}

std::shared_ptr<RasterShape> OcctScene::createRasterShape(const TopoDS_Shape &shape) {
  std::shared_ptr<RasterTodo> out = std::make_shared<RasterTodo>();
  out->id = nextId++;
  return out;
}
