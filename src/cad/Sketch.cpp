#include "Sketch.h"
#include "raymath.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>

namespace Sketch {

std::default_random_engine Point::e = std::default_random_engine();
std::default_random_engine Line::e = std::default_random_engine();

Point::Point(std::shared_ptr<GeometricElement> v) {
  this->v = v;
  std::uniform_real_distribution<float> dis(0, 1);
  pos = {
    dis(e),
    dis(e)
  };
}

Point::Point(const Point& other) {
  v = other.v;
  pos = other.pos;
  fixed = other.fixed;
}

Point::~Point() {
}

void Point::update(Vector2 diff) {
  pos.x += diff.x;
  pos.y += diff.y;
}

Line::Line(std::shared_ptr<GeometricElement> v) {
  this->v = v;
  std::uniform_real_distribution<float> dis(0, 1);
  k = dis(e);
  m = dis(e);
}

Line::Line(const Line& other) {
  v = other.v;
  k = other.k;
  m = other.m;
  fixed = other.fixed;
}

Line::~Line() {
}

void Line::update(Vector2 diff) {
  k += diff.x;
  m += diff.y;
}

float error(const Point& p1, const Point& p2, const Constraint& c) {
  switch (c.type) {
  case ConstraintType::DISTANCE:
    return std::pow(Vector2Distance(p1.pos, p2.pos) - c.value, 2);
  case ConstraintType::HORIZONTAL:
    return std::pow(p1.pos.y - p2.pos.y, 2);
  case ConstraintType::VERTICAL:
    return std::pow(p1.pos.x - p2.pos.x, 2);
  default:
    return 0;
  }
}

float error(const Point& p, const Line& l, const Constraint& c) {
  switch (c.type) {
  case ConstraintType::COINCIDENT:
    return std::pow(p.pos.x * l.k + l.m - p.pos.y, 2);
  case ConstraintType::DISTANCE:
    return std::pow((l.k * p.pos.x + l.m - p.pos.y)/std::sqrt(l.k * l.k + 1) - c.value, 2);
  default:
    return 0;
  }
}

float error(const Line& l, const Point& p, const Constraint& c) {
  return error(p, l, c);
}

float error(const Line& p1, const Line& p2, const Constraint& c) {
  // TODO
  return 0.0;
}

float error(SketchEntity* p1, SketchEntity* p2, Constraint* edge) {
  float e = 0;
  if (Point* e1 = dynamic_cast<Point*>(p1); e1 != nullptr) {
    if (Point* e2 = dynamic_cast<Point*>(p2); e2!= nullptr) {
      e = error(*e1, *e2, *edge);
    } else if (Line* e2 = dynamic_cast<Line*>(p2); e2!= nullptr) {
      e = error(*e1, *e2, *edge);
    }
  } else if (Line* e1 = dynamic_cast<Line*>(p1); e1 != nullptr) {
    if (Point* e2 = dynamic_cast<Point*>(p2); e2!= nullptr) {
      e = error(*e1, *e2, *edge);
    } else if (Line* e2 = dynamic_cast<Line*>(p2); e2!= nullptr) {
      e = error(*e1, *e2, *edge);
    }
  }
  return e;
}

Vector2 gradError(
  const Point& p1,
  const Point& p2,
  const Constraint& c
) {
  float dist = Vector2Distance(p1.pos, p2.pos);
  switch (c.type) {
  case ConstraintType::DISTANCE:
        return (Vector2) { 2*(p1.pos.x-p2.pos.x)*(dist-c.value)/dist, 2*(p1.pos.y-p2.pos.y)*(dist-c.value)/dist };
  case ConstraintType::VERTICAL:
        return (Vector2) { 2*(p1.pos.x - p2.pos.x), 0 };
  case ConstraintType::HORIZONTAL:
        return (Vector2) { 0, 2*(p1.pos.y - p2.pos.y) };
  default:
        return (Vector2) {0, 0};
  }
}

Vector2 gradError(const Point& p, const Line& l, const Constraint& c) {
  switch (c.type) {
  case ConstraintType::COINCIDENT:
    return { 2*l.k*(p.pos.x*l.k + l.m - p.pos.y), -2*(p.pos.x*l.k + l.m - p.pos.y)};
  case ConstraintType::DISTANCE:
    return {
      (2*l.k*(c.value*(-std::sqrt(l.k * l.k + 1)) + l.k*p.pos.x + l.m - p.pos.y))/(l.k*l.k + 1.0f),
      (2*(c.value * std::sqrt(l.k * l.k + 1) - l.k * p.pos.x - l.m + p.pos.y))/(l.k * l.k + 1.0f)
    };
  default:
    return { 0, 0 };
  }
}

Vector2 gradError( const Line& l, const Point& p, const Constraint& c) {
  switch (c.type) {
  case ConstraintType::COINCIDENT:
    return { 2*p.pos.x*(p.pos.x*l.k + l.m - p.pos.y), 2*(p.pos.x*l.k + l.m - p.pos.y) };
  case ConstraintType::DISTANCE:
    return {
      (2*(l.k*(p.pos.y - l.m)+p.pos.x)*(c.value*(-std::sqrt(l.k*l.k + 1)) + l.k * p.pos.x + l.m - p.pos.y))/(l.k*l.k + 1.0f),
      (2*(c.value*(-std::sqrt(l.k*l.k + 1)) + l.k*p.pos.x + l.m - p.pos.y))/(l.k*l.k + 1.0f),
    };
  default:
    return { 0, 0 };
  }
}

Vector2 gradError( const Line& p1, const Line& p2, const Constraint& c) {
  // TODO
  return { 0, 0 };
}

Vector2 gradError(SketchEntity* p1, SketchEntity* p2, Constraint* edge) {
  Vector2 diff = {0, 0};
  if (Point* e1 = dynamic_cast<Point*>(p1); e1 != nullptr) {
    if (Point* e2 = dynamic_cast<Point*>(p2); e2!= nullptr) {
      diff = gradError(*e1, *e2, *edge);
    } else if (Line* e2 = dynamic_cast<Line*>(p2); e2!= nullptr) {
      diff = gradError(*e1, *e2, *edge);
    }
  } else if (Line* e1 = dynamic_cast<Line*>(p1); e1 != nullptr) {
    if (Point* e2 = dynamic_cast<Point*>(p2); e2!= nullptr) {
      diff = gradError(*e1, *e2, *edge);
    } else if (Line* e2 = dynamic_cast<Line*>(p2); e2!= nullptr) {
      diff = gradError(*e1, *e2, *edge);
    }
  }
  return diff;
}

Realisation::Realisation() {
}

Realisation::Realisation(std::shared_ptr<ConstraintGraph> g) {
  setGraph(g);
}

Realisation::Realisation(const Realisation& other) {
  setGraph(other.g);
  stepSize = other.stepSize;
  points.clear();
  for (const Point& p: other.points) {
    points.push_back(Point(p));
  }
}

Realisation::~Realisation() {
}

float Realisation::sgdStep() {
  std::vector<Vector2> step;
  float err = 0.0f;
  for (const auto& p: points) {
    step.push_back({0.0f, 0.0f});
  }

  int total_constraints = 0;
  int i = 0;
  for (const auto& p1: points) {
    for (const auto& [edge, other]: p1.v->edges) {
        Point p2 = *findPointById(other);
          if (p1.v->isConnected(other) && std::rand() % 2 == 0) {
            float e = error(p1, p2, *edge);
            err += e;
            Vector2 grads = gradError(p1, p2, *edge);
            step[i].x -= grads.x;
            step[i].y -= grads.y;
            ++total_constraints;
          }
      }
    ++i;
  }
  err /= total_constraints;

  i = 0;
  for (auto& p: points) {
    p.pos.x += step[i].x * stepSize;
    p.pos.y += step[i].y * stepSize;
    ++i;
  }
  err /= total_constraints;

  return err;
}

Point* Realisation::findPointById(std::shared_ptr<GeometricElement> v) {
  for (Point& p: points) {
    if (p.v->id == v->id) {
      return &p;
    }
  }

  return nullptr;
}

bool Realisation::containsPointById(std::shared_ptr<GeometricElement> v) {
  return findPointById(v) != nullptr;
}

void Realisation::setGraph(std::shared_ptr<ConstraintGraph> g) {
  points.clear();
  this->g = g;

  for (const auto& v: g->vertices) {
    points.push_back(Point(v));
  }
}

void Realisation::mergeSubRealisations(Realisation* r1, Realisation* r2) {
  if ((r1 == nullptr) != (r2 == nullptr)) {
    std::cout << "A realisation always has two or no sub-realisations" << std::endl;
    exit(1);
  }
  if (r2->points.size() > r1->points.size()) {
    std::swap(r1, r2);
  }
  for (const Point& subP: r1->points) {
    Point* p = findPointById(subP.v);
    p->pos.x = subP.pos.x;
    p->pos.y = subP.pos.y;
  }
  Vector2 translation = { 0.0f, 0.0f };
  // Find one of the separation vertices to get a translation
  for (const Point& subP: r2->points) {
    Point* sepVertex = r1->findPointById(subP.v);
    if (sepVertex != nullptr) {
      // Must be a separation vertex to exist in both realisations
      translation = Vector2Subtract(subP.pos, sepVertex->pos);
      break;
    }
  }
  // It is safe to assume that a translation is always present
  for (const Point& subP: r2->points) {
    Point* p = findPointById(subP.v);
    p->pos.x += translation.x;
    p->pos.y += translation.y;
  }
}

Point* Realisation::findPoint(std::shared_ptr<GeometricElement> v) {
  for (Point& p: points) {
    if (p.v == v) {
      return &p;
    }
  }

  return nullptr;
}

Sketch::Sketch() {
}

Sketch::Sketch(std::shared_ptr<ConstraintGraph> g) {
  for (const std::shared_ptr<GeometricElement>& v: g->vertices) {
    std::shared_ptr<GeometricElement> newV = std::make_shared<GeometricElement>(*v);
    addVertex(newV);
  }

  for (const std::shared_ptr<GeometricElement>& v: g->vertices) {
    for (const auto& [edge, other] : v->edges) {
      if (!findEdgeById(edge->id)) {
      std::shared_ptr<Constraint> newC(new Constraint(*edge));

      std::shared_ptr<GeometricElement> vPrime = findVertexById(v->id);
      std::shared_ptr<GeometricElement> otherPrime = findVertexById(other->id);
      connect(vPrime, otherPrime, newC);
      }
    }
  }
}

Sketch::~Sketch() {
}


bool Sketch::contains(std::shared_ptr<GeometricElement> a) {
  return std::find(vertices.begin(), vertices.end(), a) != vertices.end();
}

std::optional<std::shared_ptr<Realisation>> Sketch::solve() {
  std::shared_ptr<STree> stree = analyze(asGraph());
  if (!stree || !stree->node) {
    return std::nullopt;
  }
  std::vector<std::pair<std::shared_ptr<STree>,Realisation>> solutionOrder;
  std::queue<std::shared_ptr<STree>> parseOrder;
  // Walk the tree in breadth-first order
  parseOrder.push(stree);
  while (!parseOrder.empty()) {
    const std::shared_ptr<STree> current = parseOrder.front();
    parseOrder.pop();
    solutionOrder.push_back(std::make_pair(current, Realisation(current->node)));
    if (current->left) {
      parseOrder.push(current->left);
    }
    if (current->right) {
      parseOrder.push(current->right);
    }
  }

  float err;
  for (int i = solutionOrder.size() - 1; i >= 0; --i) {
    Realisation* r1 = nullptr;
    Realisation* r2 = nullptr;
    if (solutionOrder[i].first->left) {
      for (int j = i + 1; j < solutionOrder.size(); ++j) {
        if (solutionOrder[j].first.get() == solutionOrder[i].first->left.get()) {
          r1 = &solutionOrder[j].second;
        }
      }
    }
    if (solutionOrder[i].first->right) {
      for (int j = i + 1; j < solutionOrder.size(); ++j) {
        if (solutionOrder[j].first.get() == solutionOrder[i].first->right.get()) {
          r2 = &solutionOrder[j].second;
        }
      }
    }
    if (r1 && r2) {
       solutionOrder[i].second.mergeSubRealisations(r1, r2);
    }

    // TODO: Error tolerance criteria. Allows for quicker termination if possible
    for (size_t j = 0; j < 10000; ++j) {
      err = solutionOrder[i].second.sgdStep();
    }
  }

  if (err < tolerance) {
    return std::make_shared<Realisation>(
      solutionOrder[0].second
    );
  } else {
    return std::nullopt;
  }
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

std::shared_ptr<GeometricElement> Sketch::findVertexById(int id) {
  for (const std::shared_ptr<GeometricElement>& v: vertices) {
    if (v->id == id) {
      return v;
    }
  }
  return std::shared_ptr<GeometricElement>(nullptr);
}


std::shared_ptr<Constraint> Sketch::findEdgeById(int id) {
  for (const std::shared_ptr<Constraint>& e: edges) {
    if (e->id == id) {
      return e;
    }
  }
  return std::shared_ptr<Constraint>(nullptr);
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

NewSketch::NewSketch() {
}

NewSketch::~NewSketch() {
}

std::shared_ptr<SketchEntity> NewSketch::findEntityById(std::shared_ptr<GeometricElement> v) {
  for (const auto& p: points) {
    if (p->v->id == v->id) {
      return p;
    }
  }

  return nullptr;
}

float NewSketch::sgdStep() {
  std::vector<Vector2> step;
  float err = 0.0f;
  for (const auto& p: points) {
    step.push_back({0.0f, 0.0f});
  }

  int total_constraints = 0;
  int i = 0;
  for (const auto& p1: points) {
    for (const auto& [edge, other]: p1->v->edges) {
        std::shared_ptr<SketchEntity> p2 = findEntityById(other);
          if (p1->v->isConnected(other) && std::rand() % 2 == 0 && !p1->fixed) {
            float e = error(p1.get(), p2.get(), edge.get());
            err += e;
            Vector2 grads = gradError(p1.get(), p2.get(), edge.get());
            step[i].x -= grads.x;
            step[i].y -= grads.y;
            ++total_constraints;
          }
      }
    ++i;
  }
  err /= total_constraints;

  i = 0;
  for (auto& p: points) {
    p->update(Vector2Scale(step[i], stepSize));
    ++i;
  }

  return err;
}

float NewSketch::totalError() {
  float total = 0.0f;
  for (const auto& p: points) {
    for (const auto& [edge, other] : p->v->edges) {
      const auto p2 = findEntityById(other);
      total += error(p.get(), p2.get(), edge.get());
    }
  }

  return total / 2.0f;
}

void NewSketch::addPoint(std::shared_ptr<Point> p) {
  points.push_back(p);
}

void NewSketch::addLine(std::shared_ptr<Line> l) {
  points.push_back(l);
}

void NewSketch::connect(
  std::shared_ptr<SketchEntity> a,
  std::shared_ptr<SketchEntity> b,
  std::shared_ptr<Constraint> c
) {
  a->v->edges.push_back(std::make_pair(c, b->v));
  b->v->edges.push_back(std::make_pair(c, a->v));
  edges.push_back(c);
}

void NewSketch::deleteEntity(std::shared_ptr<SketchEntity> a) {
  std::erase_if(points, [a](std::shared_ptr<SketchEntity> b) { return a->v->id == b->v->id; });
  for (const auto& [edge, other] : a->v->edges) {
    other->deleteEdge(a->v);
    std::erase_if(edges, [edge](std::shared_ptr<Constraint> c) { return c == edge; });
  }
}

}
