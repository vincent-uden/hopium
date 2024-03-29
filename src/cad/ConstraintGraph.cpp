#include "ConstraintGraph.h"
#include <algorithm>
#include <iostream>

int Constraint::nextId = 1;

Constraint::Constraint(ConstraintType type) {
  this->type = type;
  label = "";
  id = nextId++;
}

Constraint::Constraint(ConstraintType type, std::string label) {
  this->type = type;
  this->label = label;
  id = nextId++;
}

Constraint::Constraint(const Constraint& other) {
  this->type = other.type;
  this->flow = other.flow;
  this->id = other.id;
  this->label = other.label;
  this->value = other.value;
}

Constraint::~Constraint() {
}

int Constraint::weight() {
  int out = 0;
  switch (type) {
  case ConstraintType::COINCIDENT:
    out = 2;
    break;
  case ConstraintType::EQUAL:
    out = 1;
    break;
  case ConstraintType::PARALLEL:
    out = 1;
    break;
  case ConstraintType::PERPENDICULAR:
    out = 1;
    break;
  case ConstraintType::MIDPOINT:
    out = 1;
    break;
  case ConstraintType::COLINEAR:
    out = 1;
    break;
  case ConstraintType::DISTANCE:
    out = 1;
    break;
  case ConstraintType::ANGLE:
    out = 1;
    break;
  case ConstraintType::VIRTUAL:
    out = 0;
    break;
  case ConstraintType::HORIZONTAL:
    out = 1;
    break;
  case ConstraintType::VERTICAL:
    out = 1;
    break;
  };

  return out;
}

int Constraint::getFlow() {
  //return (type == ConstraintType::VIRTUAL) + flow;
  return flow;
}

void Constraint::setFlow(int flow) {
  //if (type != ConstraintType::VIRTUAL) {
    this->flow = flow;
  //}
}

int GeometricElement::nextId = 0;

GeometricElement::GeometricElement(GeometricType type) {
  this->type = type;
  label = "";
  id = nextId++;
}

GeometricElement::GeometricElement(GeometricType type, std::string label) {
  this->type = type;
  this->label = label;
  id = nextId++;
}

GeometricElement::GeometricElement(const GeometricElement &other) {
  this->type = other.type;
  this->label = other.label;
  this->id = other.id;
  this->explored = other.explored;

  this->edges.clear();
}

GeometricElement::~GeometricElement() {
}

bool GeometricElement::isConnected(std::shared_ptr<GeometricElement> other) {
  for (const auto& [edge, v] : edges) {
    if (v == other) {
      return true;
    }
  }
  return false;
}

bool GeometricElement::isVirtuallyConnected(std::shared_ptr<GeometricElement> other) {
  for (const auto& [edge, v] : edges) {
    if (v == other && edge->type == ConstraintType::VIRTUAL) {
      return true;
    }
  }
  return false;
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

void GeometricElement::deleteEdge(std::shared_ptr<GeometricElement> other) {
  std::erase_if(
    edges,
    [other](std::pair<std::shared_ptr<Constraint>,std::shared_ptr<GeometricElement>> p) {
      return other->id == p.second->id;
    }
  );
}

std::shared_ptr<Constraint> GeometricElement::getConnection(std::shared_ptr<GeometricElement> other) {
  for (const auto& [edge, v] : edges) {
    if (v == other) {
      return edge;
    }
  }
  return nullptr;
}

STree::STree() {
}

STree::~STree() {
}

int STree::depth() {
  int depth = 0;
  if (left) {
    depth = std::max(left->depth(), depth);
  }
  if (right) {
    depth = std::max(right->depth(), depth);
  }
  return depth + 1;
}

int STree::size() {
  int size = 0;
  if (left) {
    size += left->size();
  }
  if (right) {
    size += right->size();
  }
  return size + 1;
}

ConstraintGraph::ConstraintGraph() {
}


ConstraintGraph::~ConstraintGraph() {
}


void ConstraintGraph::addVirtualEdge(
  std::shared_ptr<GeometricElement> a,
  std::shared_ptr<GeometricElement> b
) {
  std::shared_ptr<Constraint> c(new Constraint(ConstraintType::VIRTUAL));
  std::shared_ptr<GeometricElement> v = findVertexById(a->id);
  std::shared_ptr<GeometricElement> u = findVertexById(b->id);
  connect(v, u, c);
}

int ConstraintGraph::deficit() {
  int out = 2 * vertices.size() - 3 - edges.size();
  return out;
}

void ConstraintGraph::addVertex(std::shared_ptr<GeometricElement> element) {
  vertices.push_back(element);
}

void ConstraintGraph::connect(
  std::shared_ptr<GeometricElement> a,
  std::shared_ptr<GeometricElement> b,
  std::shared_ptr<Constraint> c
) {
  a->edges.push_back(std::make_pair(c, b));
  b->edges.push_back(std::make_pair(c, a));
  edges.push_back(c);
}

void ConstraintGraph::deleteVertex(std::shared_ptr<GeometricElement> a) {
  std::erase_if(vertices, [a](std::shared_ptr<GeometricElement> b) { return a->id == b->id; });
  for (const auto& [edge, other] : a->edges) {
    other->deleteEdge(a);
    std::erase_if(edges, [edge](std::shared_ptr<Constraint> c) { return c == edge; });
  }
}

void ConstraintGraph::print() {
  for (const std::shared_ptr<GeometricElement>& vertex : vertices) {
    std::cout << vertex->label << " " << vertex->id << " Explored: " << vertex->explored << std::endl;
    for (const auto& edge : vertex->edges) {
      std::cout << "  " << edge.first->label << " " << edge.second->label << std::endl;
    }
  }
}

bool ConstraintGraph::adjacent(
  std::shared_ptr<GeometricElement> a,
  std::shared_ptr<GeometricElement> b
) {
  bool out = false;
  for (auto e : a->edges) {
    if (e.second == b) {
      out = true;
      break;
    }
  }
  return out;
}

bool ConstraintGraph::connected() {
  size_t i = 0;
  for (; i < vertices.size() - 1; i++) {
    if (!vertices[i]->explored) {
      break;
    }
  }
  // Flood fill -> If all vertices could be reached then the graph is connected
  floodFill(vertices[i]);

  bool allExplored = true;
  for (const auto& v: vertices) {
    allExplored = allExplored && v->explored;
    v->explored = false;
  }

  return allExplored;
}

bool ConstraintGraph::contains(std::shared_ptr<GeometricElement> a) {
  return std::find(vertices.begin(), vertices.end(), a) != vertices.end();
}

bool ConstraintGraph::triconnected() {
  for (size_t i = 0; i < vertices.size(); ++i) {
    for (size_t j = i + 1; j < vertices.size(); ++j) {
      std::shared_ptr<GeometricElement> s = vertices[i];
      std::shared_ptr<GeometricElement> t = vertices[j];

      if (maxFlow(s,t) < 3) {
        return false;
      }
    }
  }
  return true;
}

std::pair<std::shared_ptr<ConstraintGraph>,std::shared_ptr<ConstraintGraph>>
ConstraintGraph::splitGraphs(
    std::shared_ptr<GeometricElement> a,
    std::shared_ptr<GeometricElement> b
) {
  std::shared_ptr<ConstraintGraph> G1 = deepCopy();
  G1->deleteVertex(G1->findVertexById(a->id));
  G1->deleteVertex(G1->findVertexById(b->id));
  G1->floodFill(G1->vertices.front());
  std::shared_ptr<ConstraintGraph> G2 = G1->deepCopy();

  for (auto it = G1->vertices.begin(); it != G1->vertices.end();) {
    std::shared_ptr<GeometricElement> v = *it;
    if (v->explored) {
      G1->deleteVertex(v);
    } else {
      ++it;
    }
  }
  for (auto it = G2->vertices.begin(); it != G2->vertices.end();) {
    std::shared_ptr<GeometricElement> v = *it;
    if (!v->explored) {
      G2->deleteVertex(v);
    } else {
      ++it;
    }
  }

  std::shared_ptr<GeometricElement> a1 = std::make_shared<GeometricElement>(*a);
  std::shared_ptr<GeometricElement> b1 = std::make_shared<GeometricElement>(*b);
  std::shared_ptr<GeometricElement> a2 = std::make_shared<GeometricElement>(*a);
  std::shared_ptr<GeometricElement> b2 = std::make_shared<GeometricElement>(*b);


  G1->addVertex(a1);
  G1->addVertex(b1);
  for (const auto& [edge, other] : a->edges) {
    std::shared_ptr<GeometricElement> other1 = G1->findVertexById(other->id);
    if (other1) {
      G1->connect(a1, other1, edge);
    }
  }
  for (const auto& [edge, other] : b->edges) {
    std::shared_ptr<GeometricElement> other1 = G1->findVertexById(other->id);
    if (other1 && other1->id != a1->id) {
      G1->connect(b1, other1, edge);
    }
  }

  G2->addVertex(a2);
  G2->addVertex(b2);
  for (const auto& [edge, other] : a->edges) {
    std::shared_ptr<GeometricElement> other2 = G2->findVertexById(other->id);
    if (other2 && other2->id != b2->id) {
      G2->connect(a2, other2, edge);
    }
  }
  for (const auto& [edge, other] : b->edges) {
    std::shared_ptr<GeometricElement> other2 = G2->findVertexById(other->id);
    if (other2 && other2->id != a2->id) {
      G2->connect(b2, other2, edge);
    }
  }
  G1->resetExploration();
  G2->resetExploration();

  return std::make_pair(G1, G2);
}

int ConstraintGraph::maxFlow(
  std::shared_ptr<GeometricElement> source,
  std::shared_ptr<GeometricElement> sink
) {
  // Ford-Fulkerson algorithm
  int flow = 0;
  std::optional<std::vector<std::shared_ptr<Constraint>>> path;
  while ( (path = breadthFirstSearch(source, sink)).has_value() ) {
    for (const auto& e: path.value()) {
      e->setFlow(e->getFlow() + 1);
    }
    flow++;
  }

  for (const auto& e: edges) {
    e->setFlow(0);
  }

  return flow;
}

std::pair<std::shared_ptr<GeometricElement>,std::shared_ptr<GeometricElement>>
ConstraintGraph::separatingVertices() {
  // Dumb implementation
  for (size_t i = 0; i < vertices.size(); ++i) {
    for (size_t j = i + 1; j < vertices.size(); ++j) {
      std::shared_ptr<GeometricElement> a = vertices[i];
      std::shared_ptr<GeometricElement> b = vertices[j];

      a->explored = true;
      b->explored = true;
      bool conn = connected();
      a->explored = false;
      b->explored = false;

      if (!conn) {
        return std::make_pair(a, b);
      }
    }
  }

  return std::make_pair(nullptr, nullptr);
}

std::shared_ptr<ConstraintGraph> ConstraintGraph::deepCopy() {
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

std::shared_ptr<GeometricElement> ConstraintGraph::findVertexById(int id) {
  for (const std::shared_ptr<GeometricElement>& v: vertices) {
    if (v->id == id) {
      return v;
    }
  }
  return std::shared_ptr<GeometricElement>(nullptr);
}


std::shared_ptr<Constraint> ConstraintGraph::findEdgeById(int id) {
  for (const std::shared_ptr<Constraint>& e: edges) {
    if (e->id == id) {
      return e;
    }
  }
  return std::shared_ptr<Constraint>(nullptr);
}

std::optional<std::vector<std::shared_ptr<Constraint>>>
ConstraintGraph::breadthFirstSearch(
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
      if (!other->explored && edge->getFlow() == 0) {
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

void ConstraintGraph::floodFill(std::shared_ptr<GeometricElement> start) {
  std::queue<std::shared_ptr<GeometricElement>> Q;
  Q.push(start);
  start->explored = true;

  std::shared_ptr<GeometricElement> v;
  while (!Q.empty()) {
    v = Q.front();
    Q.pop();

    for (const auto& [edge, other] : v->edges) {
      //if (!other->explored && edge->type != ConstraintType::VIRTUAL) {
      if (!other->explored) {
        Q.push(other);
        other->explored = true;
        other->parent = v;
        other->parentEdge = edge;
      }
    }
  }
}

void ConstraintGraph::resetExploration() {
  for (const std::shared_ptr<GeometricElement>& v: vertices) {
    v->explored = false;
  }
}

std::shared_ptr<STree> analyze(std::shared_ptr<ConstraintGraph> G) {
  // Decomposition algorithm: Joan-Arinyo, Soto-Riera, Vila-Marta & Vilaplana-Pastó
  std::shared_ptr<STree> out = std::make_shared<STree>();

  if (G->triconnected() || G->vertices.size() == 3) {
    out->node = G;
  } else {
    std::pair<std::shared_ptr<GeometricElement>, std::shared_ptr<GeometricElement>> sepVertices = G->separatingVertices();
    std::shared_ptr<GeometricElement> a = sepVertices.first;
    std::shared_ptr<GeometricElement> b = sepVertices.second;
    std::pair<std::shared_ptr<ConstraintGraph>, std::shared_ptr<ConstraintGraph>> Gs = G->splitGraphs(a, b);
    std::shared_ptr<ConstraintGraph> G1 = Gs.first;
    std::shared_ptr<ConstraintGraph> G2 = Gs.second;
    if (G1->deficit() > G2->deficit()) {
      G1->addVirtualEdge(a, b);
    } else {
      G2->addVirtualEdge(a, b);
    }
    if (G1->deficit() + G2->deficit() != G->deficit()) {
      exit(1);
    }
    out->node = G;
    out->left = analyze(G1);
    out->right = analyze(G2);
  }

  return out;
}

