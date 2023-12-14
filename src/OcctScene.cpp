#include "OcctScene.h"
#include <BRepMesh_IncrementalMesh.hxx>
#include <ShapeAnalysis_Wire.hxx>
#include <ShapeExtend_Status.hxx>
#include <TopoDS_Solid.hxx>
#include <gp_Pln.hxx>

OcctScene::OcctScene() {
}

OcctScene::~OcctScene() {
}

void OcctScene::createPoint(double x, double y, double z) {
  points.push_back(gp_Pnt(x, y, z));
  ids.push_back(nextId++);
}

void OcctScene::createLine(gp_Pnt& p1, gp_Pnt& p2, double snapThreshold) {
  // Snap to existing points if they exist
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

  bool connected = false;
  int i = 0;
  for (const auto& [id, shape]: shapes) {
    if (shape.ShapeType() == TopAbs_EDGE) {
      TopoDS_Edge existingEdge = TopoDS::Edge(shape);
      std::pair<gp_Pnt, gp_Pnt> existingPoints = getEdgePoints(existingEdge);
      if (
        existingPoints.first.Distance(*start) < snapThreshold ||
        existingPoints.first.Distance(*end) < snapThreshold ||
        existingPoints.second.Distance(*start) < snapThreshold ||
        existingPoints.second.Distance(*end) < snapThreshold)
      {
        TopoDS_Wire combined = BRepBuilderAPI_MakeWire(existingEdge, edge);

        shapes.erase(std::next(shapes.begin(), i));
        shapes.push_back(std::pair(nextId++, combined));

        connected = true;
        break;
      }
    } else if (shape.ShapeType() == TopAbs_WIRE) {
      TopoDS_Wire wire = TopoDS::Wire(shape);
      BRepBuilderAPI_MakeWire wireMaker(wire);
      wireMaker.Add(edge);
      if (wireMaker.IsDone()) {
        wire = wireMaker.Wire();
        gp_Pnt origin(0.0, 0.0, 0.0);
        gp_Dir normal(0.0, 1.0, 0.0);
        gp_Pln plane(origin, normal);
        TopoDS_Face f = BRepBuilderAPI_MakeFace(plane);
        ShapeAnalysis_Wire checkWire;
        checkWire.Load(wire);
        checkWire.SetFace(f);
        checkWire.SetPrecision(1.e-3);
        ShapeFix_Wire wireFix;
        wireFix.Load(wire);
        wireFix.SetFace(f);
        wireFix.SetPrecision(1.e-3);

        wireFix.Perform();
        wire = wireFix.Wire();

        shapes.erase(std::next(shapes.begin(), i));
        shapes.push_back(std::pair(nextId++,wire));
        connected = true;

        checkWire.Load(wire);
        checkWire.SetFace(f);

        if (isWireCyclic(wire)) {
          BRepBuilderAPI_MakeFace faceMaker(wire);
          if (faceMaker.IsDone()) {
            shapes.push_back(std::pair(nextId++, (TopoDS_Face) faceMaker));
          }
        }
        break;
      }
    }
    ++i;
  }

  if (!connected) {
    shapes.push_back(std::pair(nextId++, edge));
  }
}

void OcctScene::dumpShapes() {
  std::cout << "Dumping shapes:" << std::endl;
  for (const auto& [id, shape]: shapes) {
    std::cout << "Shape: " << shapeType(shape.ShapeType()) << std::endl;
    if (shape.ShapeType() == TopAbs_EDGE) {
      for (TopExp_Explorer ex(shape, TopAbs_VERTEX); ex.More(); ex.Next()) {
        auto c = BRep_Tool::Pnt(TopoDS::Vertex(ex.Current()));
        std::cout << c.X() << " " << c.Y() << " " << c.Z() << std::endl;
      }
    } else if (shape.ShapeType() == TopAbs_WIRE) {
      for (TopExp_Explorer ex(shape, TopAbs_VERTEX); ex.More(); ex.Next()) {
        auto c = BRep_Tool::Pnt(TopoDS::Vertex(ex.Current()));
        std::cout << c.X() << " " << c.Y() << " " << c.Z() << std::endl;
      }
    }
  }
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
  for (const auto& [id, shape] : shapes) {
    if (shape.ShapeType() == TopAbs_EDGE) {
      TopoDS_Edge edge = TopoDS::Edge(shape);
      for (std::shared_ptr<RasterShape>& rs:  createRasterShape(id, edge)) {
        rasterShapes.push_back(rs);
      }
    } else if (shape.ShapeType() == TopAbs_WIRE) {
      TopoDS_Wire wire = TopoDS::Wire(shape);
      for (std::shared_ptr<RasterShape>& rs:  createRasterShape(id, wire)) {
        rasterShapes.push_back(rs);
      }
    } else if (shape.ShapeType() == TopAbs_FACE) {
      TopoDS_Face face = TopoDS::Face(shape);
      for (std::shared_ptr<RasterShape>& rs:  createRasterShape(id, face)) {
        rasterShapes.push_back(rs);
      }
    }
  }

  return rasterShapes;
}

std::vector<std::shared_ptr<RasterShape>> OcctScene::createRasterShape(const size_t& id, const TopoDS_Edge &shape) {
  std::pair<gp_Pnt, gp_Pnt> points = getEdgePoints(shape);

  std::shared_ptr<RasterLine> line = std::shared_ptr<RasterLine>(new RasterLine(
        Vector3 {
          static_cast<float>(points.first.X()),
          static_cast<float>(points.first.Y()),
          static_cast<float>(points.first.Z())
        },
        Vector3 {
          static_cast<float>(points.second.X()),
          static_cast<float>(points.second.Y()),
          static_cast<float>(points.second.Z())
        }
  ));
  line->id = id;
  std::vector<std::shared_ptr<RasterShape>> out;
  out.push_back(line);
  return out;
}

std::vector<std::shared_ptr<RasterShape>> OcctScene::createRasterShape(const size_t& id, const TopoDS_Wire& shape) {
  std::vector<std::shared_ptr<RasterShape>> out;
  for (TopExp_Explorer ex(shape, TopAbs_EDGE); ex.More(); ex.Next()) {
    TopoDS_Edge edge = TopoDS::Edge(ex.Current());
    for (std::shared_ptr<RasterShape>& rs: createRasterShape(id, edge)) {
      out.push_back(rs);
    }
  }
  return out;
}

std::vector<std::shared_ptr<RasterShape>> OcctScene::createRasterShape(const size_t& id, const TopoDS_Face &shape) {
  std::vector<std::shared_ptr<RasterShape>> out;
  std::vector<Vector3> vertices;

  BRepMesh_IncrementalMesh mesh(shape, 1.e-5);
  mesh.Perform();
  TopLoc_Location loc = shape.Location();
  Handle(Poly_Triangulation) triangulation = BRep_Tool::Triangulation(shape, loc);
  for (int i = 0; i < triangulation->NbTriangles(); i++) {
    Poly_Triangle tri = triangulation->Triangle(i+1);

    for (int j = 1; j < 4; j++) {
      int cornerIndex = tri.Value(j);
      gp_Pnt p = triangulation->Node(cornerIndex);
      vertices.push_back(Vector3 {
        static_cast<float>(p.X()),
        static_cast<float>(p.Y()),
        static_cast<float>(p.Z())
      });

    }
  }

  std::shared_ptr<RasterFace> face(new RasterFace(vertices));
  face->id = id;
  out.push_back(face);

  return out;
}

std::vector<std::shared_ptr<RasterShape>> OcctScene::createRasterShape(const size_t& id, const TopoDS_Solid &shape) {
  std::vector<std::shared_ptr<RasterShape>> out;

  IMeshTools_Parameters params;
  params.Deflection = 0.01;
  params.Angle = 0.1;
  params.Relative = Standard_False;
  params.InParallel = Standard_True;
  params.MinSize = Precision::Confusion();
  params.InternalVerticesMode = Standard_True;
  params.ControlSurfaceDeflection = Standard_True;

  BRepMesh_IncrementalMesh mesh(shape, params);
  mesh.Perform();
  TopLoc_Location loc = shape.Location();

  for (TopExp_Explorer ex(shape, TopAbs_FACE); ex.More(); ex.Next()) {
    TopoDS_Face face = TopoDS::Face(ex.Current());
    TopLoc_Location loc = face.Location();
    Handle(Poly_Triangulation) triangulation = BRep_Tool::Triangulation(face, loc);

    // TODO: Continue here once we can serialize scenes
  }

  std::shared_ptr<RasterSolid> solid(new RasterSolid());
  solid->id = id;
  out.push_back(solid);

  return out;
}

std::vector<std::shared_ptr<RasterShape>> OcctScene::createRasterShape(const size_t& id, const TopoDS_Shape &shape) {
  std::shared_ptr<RasterTodo> todoShape = std::make_shared<RasterTodo>();
  todoShape->id = id;
  std::vector<std::shared_ptr<RasterShape>> out;
  out.push_back(todoShape);
  return out;
}

std::pair<gp_Pnt, gp_Pnt> OcctScene::getEdgePoints(const TopoDS_Edge& edge) {
  std::vector<gp_Pnt> points;
  for (TopExp_Explorer ex(edge, TopAbs_VERTEX); ex.More(); ex.Next()) {
    points.push_back(BRep_Tool::Pnt(TopoDS::Vertex(ex.Current())));
  }

  return std::pair<gp_Pnt, gp_Pnt>(points[0], points[1]);
}

std::optional<size_t> OcctScene::idContainingPoint(double x, double y, double z) {
  for (const auto& [id, shape]: shapes) {
    if (shape.ShapeType() == TopAbs_FACE) {
      TopoDS_Face face = TopoDS::Face(shape);
      gp_Pnt p(x, y, z);
      BRepClass_FaceClassifier classifier(face, p, 1.e-3);
      if ((classifier.State() == TopAbs_IN)) {
        return std::optional(id);
      }
    }
  }

  return std::nullopt;
}

std::string OcctScene::shapeType(const TopAbs_ShapeEnum& shapeType) {
  switch (shapeType) {
    case TopAbs_VERTEX:
      return "Vertex";
    case TopAbs_EDGE:
      return "Edge";
    case TopAbs_WIRE:
      return "Wire";
    case TopAbs_FACE:
      return "Face";
    case TopAbs_SHELL:
      return "Shell";
    case TopAbs_COMPOUND:
      return "Compound";
    case TopAbs_SOLID:
      return "Solid";
    default:
      return "Unknown";
  }
}

bool OcctScene::isWireCyclic(const TopoDS_Wire& wire) {
  TopoDS_Edge firstEdge;
  TopoDS_Edge lastEdge;

  int i = 0;
  for (TopExp_Explorer ex(wire, TopAbs_EDGE); ex.More(); ex.Next()) {
    if (i == 0) {
      firstEdge = TopoDS::Edge(ex.Current());
    }
    lastEdge = TopoDS::Edge(ex.Current());
    ++i;
  }

  std::pair<gp_Pnt, gp_Pnt> firstPoints = getEdgePoints(firstEdge);
  std::pair<gp_Pnt, gp_Pnt> lastPoints = getEdgePoints(lastEdge);
  return (
      firstPoints.first.SquareDistance(lastPoints.first) < 1.e-6 ||
      firstPoints.first.SquareDistance(lastPoints.second) < 1.e-6 ||
      firstPoints.second.SquareDistance(lastPoints.first) < 1.e-6 ||
      firstPoints.second.SquareDistance(lastPoints.second) < 1.e-6
  );
}
