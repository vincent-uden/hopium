#include "Sketch.h"
#include "raymath.h"
#include <iostream>
#include <memory>

namespace Sketch {

std::default_random_engine Point::e = std::default_random_engine();

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
}

Point::~Point() {
}

float error(const Point& p1, const Point& p2, const Constraint& c) {
  switch (c.type) {
  case ConstraintType::DISTANCE:
    return std::pow(Vector2Distance(p1.pos, p2.pos) - c.value, 2);
  case ConstraintType::VERTICAL:
    return std::pow(p1.pos.x - p2.pos.x, 2);
  case ConstraintType::HORIZONTAL:
    return std::pow(p1.pos.y - p2.pos.y, 2);
  default:
    return 0;
  }
}

// What is the gradient of this thing?? One output per coordinate in the inputs?
std::pair<Vector2,Vector2> gradError(
  const Point& p1,
  const Point& p2,
  const Constraint& c
) {
  float dist = Vector2Distance(p1.pos, p2.pos);
  switch (c.type) {
  case ConstraintType::DISTANCE:
    return std::make_pair(
        (Vector2) { 2*(p1.pos.x-p2.pos.x)*(dist-c.value)/dist, 2*(p1.pos.y-p2.pos.y)*(dist-c.value)/dist },
        (Vector2) { -2*(p1.pos.x-p2.pos.x)*(dist-c.value)/dist, -2*(p1.pos.y-p2.pos.y)*(dist-c.value)/dist }
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

  // TODO: Dont iterate twice over all constraints
  int total_constraints = 0;
  for (int b = 0; b < BATCH_SIZE; ++b) {
    for (size_t i = 0; i < points.size(); ++i) {
      const Point& p = points[i];
      for (const auto& [edge, other]: p.v->edges) {
        Point otherP = *findPoint(other);
        float e = error(p, otherP, *edge.get());
        err += e;
        std::pair<Vector2, Vector2> grads = gradError(p, otherP, *edge.get());
        step[i].x -= grads.first.x / BATCH_SIZE;
        step[i].y -= grads.first.y / BATCH_SIZE;
        ++total_constraints;
      }
    }
  }
  err /= total_constraints;

  size_t i = 0;
  for (auto& p: points) {
    p.pos.x += step[i].x * stepSize;
    p.pos.y += step[i].y * stepSize;
    ++i;
  }

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

void Realisation::setGraph(std::shared_ptr<ConstraintGraph> g) {
  points.clear();
  this->g = g;

  for (const auto& v: g->vertices) {
    points.push_back(Point(v));
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
  // Walk the tree
  // Solve sub trees in order using Stochastic Gradient Descent
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
    for (size_t j = 0; j < 1000; ++j) {
      err = solutionOrder[i].second.sgdStep();
    }
  }

  if (err < tolerance) {
    return std::make_shared<Realisation>(
      solutionOrder[solutionOrder.size() - 1].second
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
