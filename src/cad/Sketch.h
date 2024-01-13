#ifndef UDEN_SKETCH
#define UDEN_SKETCH

#include <algorithm>
#include <random>
#include <utility>

#include "raylib.h"
#include "raymath.h"

#include "ConstraintGraph.h"


namespace Sketch {

class Point {
public:
  Point(std::shared_ptr<GeometricElement> v);
  ~Point();

  Vector2 pos;
  std::shared_ptr<GeometricElement> v;
};

// TODO: These need to be mult-dimensional
float error(Point& p1, Point& p2, Constraint& c);
std::pair<Vector2,Vector2> gradError(Point& p1, Point& p2, Constraint& c);

// TODO: Create a comprehensive solution proposition class

class Realisation {
public:
  Realisation();
  ~Realisation();

  void setGraph(std::shared_ptr<ConstraintGraph> g);

  std::vector<Point> points;

private:
  std::shared_ptr<ConstraintGraph> g;
};

class Sketch {
public:
  Sketch();
  ~Sketch();

  bool contains(std::shared_ptr<GeometricElement> a);
  bool solve();
  int deficit();
  std::shared_ptr<ConstraintGraph> asGraph();
  void addVertex(std::shared_ptr<GeometricElement> element);
  void connect(std::shared_ptr<GeometricElement> a, std::shared_ptr<GeometricElement> b, std::shared_ptr<Constraint> c);
  void deleteVertex(std::shared_ptr<GeometricElement> a);

private:
  std::vector<std::shared_ptr<GeometricElement>> vertices;
  std::vector<std::shared_ptr<Constraint>> edges;
};

}

#endif
