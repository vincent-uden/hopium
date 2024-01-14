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

private:
  static std::default_random_engine e;
};

float error(const Point& p1, const Point& p2, const Constraint& c);
std::pair<Vector2,Vector2> gradError(
  const Point& p1,
  const Point& p2,
  const Constraint& c
);

class Realisation {
public:
  Realisation();
  Realisation(std::shared_ptr<ConstraintGraph> g);
  ~Realisation();

  float sgdStep();
  void setGraph(std::shared_ptr<ConstraintGraph> g);

  std::vector<Point> points;

private:
  Point* findPoint(std::shared_ptr<GeometricElement> v);

  const int BATCH_SIZE = 128;
  float stepSize = 0.1;
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

  float tolerance = 1e-9;
};

}

#endif
