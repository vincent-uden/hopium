#ifndef UDEN_CONSTRAINT_GRAPH
#define UDEN_CONSTRAINT_GRAPH

#include <memory>
#include <optional>
#include <vector>
#include <queue>

enum ConstraintType {
  COINCIDENT,
  EQUAL,
  PARALLEL,
  PERPENDICULAR,
  MIDPOINT,
  COLINEAR,
  DISTANCE,
  ANGLE,
  VIRTUAL,
};

class Constraint {
public:
  Constraint(ConstraintType type);
  ~Constraint();

  int weight();

private:
  ConstraintType type;
};

enum GeometricType {
  POINT,
  LINE,
};

class GeometricElement {
public:
  GeometricElement(GeometricType type);
  ~GeometricElement();

  int weight();

  std::vector<std::pair<std::shared_ptr<Constraint>,std::shared_ptr<GeometricElement>>> edges;

  std::shared_ptr<GeometricElement> parent;
  std::shared_ptr<Constraint> parentEdge;
  bool explored = false;

private:
  GeometricType type;
};

class STree {
public:
  STree();
  ~STree();

  std::shared_ptr<STree> left = nullptr;
  std::shared_ptr<STree> right = nullptr;
};

class ConstraintGraph {
public:
  ConstraintGraph();
  ~ConstraintGraph();

  STree analyze();
  int deficit();
  std::optional<std::vector<std::shared_ptr<Constraint>>> breadthFirstSearch(std::shared_ptr<GeometricElement> start, std::shared_ptr<GeometricElement> end);
  void addVertex(std::shared_ptr<GeometricElement> element);
  void addVirtualEdge(std::shared_ptr<GeometricElement> a, std::shared_ptr<GeometricElement> b);
  void connect(std::shared_ptr<GeometricElement> a, std::shared_ptr<GeometricElement> b, std::shared_ptr<Constraint> c);

protected:
  std::vector<std::shared_ptr<GeometricElement>> vertices;
  std::vector<std::shared_ptr<Constraint>> edges;

  bool triconnected();
  std::pair<ConstraintGraph,ConstraintGraph> separatingGraphs();
  int maxFlow(std::shared_ptr<GeometricElement> source, std::shared_ptr<GeometricElement> sink);
};


#endif
