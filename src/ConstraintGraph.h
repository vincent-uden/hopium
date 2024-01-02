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
  Constraint(ConstraintType type, std::string label);
  Constraint(const Constraint& other);
  ~Constraint();

  int weight();

  ConstraintType type;
  int flow = 0;
  int id;
  std::string label;

private:
  static int nextId;
};

enum GeometricType {
  POINT,
  LINE,
};

class GeometricElement {
public:
  GeometricElement(GeometricType type);
  GeometricElement(GeometricType type, std::string label);
  GeometricElement(const GeometricElement &other);
  ~GeometricElement();

  int weight();
  void deleteEdge(std::shared_ptr<GeometricElement> other);

  std::vector<std::pair<std::shared_ptr<Constraint>,std::shared_ptr<GeometricElement>>> edges;

  GeometricType type;
  bool explored = false;
  int id;
  std::shared_ptr<Constraint> parentEdge;
  std::shared_ptr<GeometricElement> parent;
  std::string label;

private:
  static int nextId;
};

class ConstraintGraph;

class STree {
public:
  STree();
  ~STree();

  std::shared_ptr<STree> left = nullptr;
  std::shared_ptr<STree> right = nullptr;
  std::shared_ptr<ConstraintGraph> node = nullptr;
};

class ConstraintGraph {
public:
  ConstraintGraph();
  ~ConstraintGraph();


  bool adjacent(std::shared_ptr<GeometricElement> a, std::shared_ptr<GeometricElement> b);
  bool connected();
  bool contains(std::shared_ptr<GeometricElement> a);
  bool triconnected();
  int deficit();
  int maxFlow(std::shared_ptr<GeometricElement> source, std::shared_ptr<GeometricElement> sink);
  std::optional<std::vector<std::shared_ptr<Constraint>>> breadthFirstSearch(std::shared_ptr<GeometricElement> start, std::shared_ptr<GeometricElement> end);
  std::pair<std::shared_ptr<ConstraintGraph>,std::shared_ptr<ConstraintGraph>> separatingGraphs(std::shared_ptr<GeometricElement> a, std::shared_ptr<GeometricElement> b);
  std::pair<std::shared_ptr<GeometricElement>,std::shared_ptr<GeometricElement>> separatingVertices();
  std::shared_ptr<ConstraintGraph> deepCopy();
  std::shared_ptr<GeometricElement> findVertexById(int id);
  std::shared_ptr<Constraint> findEdgeById(int id);
  void addVertex(std::shared_ptr<GeometricElement> element);
  void addVirtualEdge(std::shared_ptr<GeometricElement> a, std::shared_ptr<GeometricElement> b);
  void connect(std::shared_ptr<GeometricElement> a, std::shared_ptr<GeometricElement> b, std::shared_ptr<Constraint> c);
  void deleteVertex(std::shared_ptr<GeometricElement> a);
  void print();

  // TODO: Move back to private
  std::vector<std::shared_ptr<GeometricElement>> vertices;
  std::vector<std::shared_ptr<Constraint>> edges;

protected:
  void floodFill(std::shared_ptr<GeometricElement> start);
  void resetExploration();
};


std::shared_ptr<STree> analyze(std::shared_ptr<ConstraintGraph> G);
#endif
