#include "Sketch.h"
#include "raymath.h"

namespace Sketch {

Point::Point(std::shared_ptr<GeometricElement> v) {
  this->v = v;
  std::default_random_engine e;
  std::uniform_real_distribution<float> dis(0, 1);
  pos = {
    dis(e),
    dis(e)
  };
}

Point::~Point() {
}

float error(Point& p1, Point& p2, Constraint& c) {
  switch (c.type) {
  case ConstraintType::DISTANCE:
    return std::pow(Vector2Distance(p1.pos, p2.pos) - c.value, 2);
  case ConstraintType::HORIZONTAL:
    return std::pow(p1.pos.x - p2.pos.x, 2);
  case ConstraintType::VERTICAL:
    return std::pow(p1.pos.y - p2.pos.y, 2);
  default:
    return 0;
  }
}

// What is the gradient of this thing?? One output per coordinate in the inputs?
std::pair<Vector2,Vector2> gradError(Point& p1, Point& p2, Constraint& c) {
  float dist = Vector2Distance(p1.pos, p2.pos);
  switch (c.type) {
  case ConstraintType::DISTANCE:
    return std::make_pair(
        (Vector2) { (p1.pos.x - p2.pos.x)/dist, (p1.pos.y - p2.pos.y)/dist },
        (Vector2) { -(p1.pos.x - p2.pos.x)/dist, -(p1.pos.y - p2.pos.y)/dist }
    );
  case ConstraintType::VERTICAL:
    return std::make_pair(
        (Vector2) { 2*(p1.pos.x - p2.pos.x), 0 },
        (Vector2) { -2*(p1.pos.x - p2.pos.x), 0 }
    );
  case ConstraintType::HORIZONTAL:
    return std::make_pair(
        (Vector2) { 0, 2*(p1.pos.y - p2.pos.y) },
        (Vector2) { 0, -2*(p1.pos.y - p2.pos.y) }
    );
  default:
    return std::make_pair(
        (Vector2) {0, 0},
        (Vector2) {0, 0}
        );
  }
}

Realisation::Realisation() {
}

Realisation::~Realisation() {
}

void Realisation::setGraph(std::shared_ptr<ConstraintGraph> g) {
  points.clear();
  this->g = g;

  for (const auto& v: g->vertices) {
    points.push_back(Point(v));
  }
}


Sketch::Sketch() {
}

Sketch::~Sketch() {
}


bool Sketch::contains(std::shared_ptr<GeometricElement> a) {
  return std::find(vertices.begin(), vertices.end(), a) != vertices.end();
}

bool Sketch::solve() {
  if (deficit() != 0) {
    return false;
  }

  std::shared_ptr<STree> stree = analyze(asGraph());
  if (!stree || !stree->node) {
    return false;
  }

  return false;
}

int Sketch::deficit() {
  return 2 * vertices.size() - 3 - edges.size();
}
std::shared_ptr<ConstraintGraph> Sketch::asGraph() {
  std::shared_ptr<ConstraintGraph> out = std::make_shared<ConstraintGraph>();
  for (const std::shared_ptr<GeometricElement>& v: vertices) {
    std::shared_ptr<GeometricElement> newV = std::make_shared<GeometricElement>(*v);
    out->addVertex(newV);
  }

  for (const std::shared_ptr<GeometricElement>& v: vertices) {
    for (const auto& [edge, other] : v->edges) {
      if (!out->findEdgeById(edge->id)) {
      std::shared_ptr<Constraint> newC(new Constraint(*edge));

      std::shared_ptr<GeometricElement> vPrime = out->findVertexById(v->id);
      std::shared_ptr<GeometricElement> otherPrime = out->findVertexById(other->id);
      out->connect(vPrime, otherPrime, newC);
      }
    }
  }

  return out;
}


void Sketch::addVertex(std::shared_ptr<GeometricElement> element) {
  vertices.push_back(element);
}

void Sketch::connect(
  std::shared_ptr<GeometricElement> a,
  std::shared_ptr<GeometricElement> b,
  std::shared_ptr<Constraint> c
) {
  a->edges.push_back(std::make_pair(c, b));
  b->edges.push_back(std::make_pair(c, a));
  edges.push_back(c);
}

void Sketch::deleteVertex(std::shared_ptr<GeometricElement> a) {
  std::erase_if(vertices, [a](std::shared_ptr<GeometricElement> b) { return a->id == b->id; });
  for (const auto& [edge, other] : a->edges) {
    other->deleteEdge(a);
    std::erase_if(edges, [edge](std::shared_ptr<Constraint> c) { return c == edge; });
  }
}

}
