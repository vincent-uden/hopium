#include "ConstraintGraph.h"
#include <algorithm>

Constraint::Constraint(ConstraintType type) {
  this->type = type;
  label = "";
}

Constraint::Constraint(ConstraintType type, std::string label) {
  this->type = type;
  this->label = label;
}

Constraint::~Constraint() {
}

int Constraint::weight() {
  int out = 0;
  switch (type) {
  case COINCIDENT:
    out = 2;
    break;
  case EQUAL:
    out = 1;
    break;
  case PARALLEL:
    out = 1;
    break;
  case PERPENDICULAR:
    out = 1;
    break;
  case MIDPOINT:
    out = 1;
    break;
  case COLINEAR:
    out = 1;
    break;
  case DISTANCE:
    out = 1;
    break;
  case ANGLE:
    out = 1;
    break;
  case VIRTUAL:
    out = 0;
    break;
  };

  return out;
}

GeometricElement::GeometricElement(GeometricType type) {
  this->type = type;
  label = "";
}

GeometricElement::GeometricElement(GeometricType type, std::string label) {
  this->type = type;
  this->label = label;
}

GeometricElement::~GeometricElement() {
}

int GeometricElement::weight() {
  int out = 0;
  switch (type) {
  case POINT:
    out = 2; // (x, y)
    break;
  case LINE:
    out = 2; // y = kx + m => (k, m)
    break;
  }

  return out;
}

STree::STree() {
}

STree::~STree() {
}

ConstraintGraph::ConstraintGraph() {
}

ConstraintGraph::~ConstraintGraph() {
}

STree ConstraintGraph::analyze() {
  STree out;
  return out;
}

void ConstraintGraph::addVirtualEdge(
  std::shared_ptr<GeometricElement> a,
  std::shared_ptr<GeometricElement> b
) {
  std::shared_ptr<Constraint> c = std::make_shared<Constraint>(ConstraintType::VIRTUAL);
  a->edges.push_back(std::make_pair(c, b));
  b->edges.push_back(std::make_pair(c, a));
}

int ConstraintGraph::deficit() {
  return -1;
}

void ConstraintGraph::addVertex(std::shared_ptr<GeometricElement> element) {
  vertices.push_back(element);
}

void ConstraintGraph::connect(std::shared_ptr<GeometricElement> a, std::shared_ptr<GeometricElement> b, std::shared_ptr<Constraint> c) {
  a->edges.push_back(std::make_pair(c, b));
  b->edges.push_back(std::make_pair(c, a));
}

bool ConstraintGraph::triconnected() {
  return false;
}


std::pair<ConstraintGraph,ConstraintGraph> ConstraintGraph::separatingGraphs() {
  std::pair<ConstraintGraph,ConstraintGraph> out;
  return out;
}

int ConstraintGraph::maxFlow(
  std::shared_ptr<GeometricElement> source,
  std::shared_ptr<GeometricElement> sink
) {
  // Ford-Fulkerson algorithm
  std::vector<int> flow;
  for (const auto& e: edges) {
    flow.push_back(0);
  }

  return -1;
}

std::optional<std::vector<std::shared_ptr<Constraint>>> ConstraintGraph::breadthFirstSearch(
  std::shared_ptr<GeometricElement> start,
  std::shared_ptr<GeometricElement> end
) {
  std::queue<std::shared_ptr<GeometricElement>> Q;
  Q.push(start);
  start->explored = true;

  std::shared_ptr<GeometricElement> v;
  while (!Q.empty()) {
    v = Q.front();
    Q.pop();
    if (v.get() == end.get()) {
      break;
    }

    for (const auto& [edge, other] : v->edges) {
      if (!other->explored) {
        Q.push(other);
        other->explored = true;
        other->parent = v;
        other->parentEdge = edge;
      }
    }
  }

  for (const auto& v: vertices) {
    v->explored = false;
  }

  if (v.get() == end.get()) {
    std::vector<std::shared_ptr<Constraint>> out;
    std::shared_ptr<GeometricElement> current = end;
    while (current != start) {
      out.push_back(current->parentEdge);
      current = current->parent;
    }
    std::reverse(out.begin(), out.end());
    return out;
  }

  return std::nullopt;
}
