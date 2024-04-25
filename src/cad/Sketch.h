#ifndef UDEN_SKETCH
#define UDEN_SKETCH

#include <algorithm>
#include <random>
#include <utility>

#include "raylib.h"
#include "raymath.h"

#include "ConstraintGraph.h"


namespace Sketch {

class SketchEntity {
public:
  virtual void update(Vector2 diff) = 0;
  virtual int selectionPriority() = 0;
  bool selectionOverrides(SketchEntity* other);

  bool fixed = false;
  std::shared_ptr<GeometricElement> v;
  bool draw = true;
};

class Point: public SketchEntity {
public:
  Point(std::shared_ptr<GeometricElement> v);
  Point(const Point& other);
  ~Point();

  void update(Vector2 diff) override;
  int selectionPriority() override;

  Vector2 pos;

private:
  static std::default_random_engine e;
};

class Line: public SketchEntity {
public:
  Line(std::shared_ptr<GeometricElement> v);
  Line(const Line& other);
  ~Line();

  void update(Vector2 diff) override;
  int selectionPriority() override;

  float k;
  float m;

private:
  static std::default_random_engine e;
};

// Cannot be adjusted themselves. Are instead guided by underlying entities
class GuidedEntity {
public:
  virtual ~GuidedEntity() = default;
};

class TrimmedLine: public GuidedEntity {
public:
  TrimmedLine(
    std::shared_ptr<Point> p1,
    std::shared_ptr<Point> p2,
    std::shared_ptr<Line> line
  );
  TrimmedLine(const TrimmedLine& other);
  ~TrimmedLine() override;

  std::shared_ptr<Point> start;
  std::shared_ptr<Point> end;
  std::shared_ptr<Line> line;
};

float error(const Point& p1, const Point& p2, const Constraint& c);
float error(const Point& p1, const Line& p2, const Constraint& c);
float error(const Line& p1, const Point& p2, const Constraint& c);
float error(const Line& p1, const Line& p2, const Constraint& c);
float error(SketchEntity* e1, SketchEntity* e2, const Constraint* c);
Vector2 gradError( const Point& p1, const Point& p2, const Constraint& c);
Vector2 gradError( const Point& p1, const Line& p2, const Constraint& c);
Vector2 gradError( const Line& p1, const Point& p2, const Constraint& c);
Vector2 gradError( const Line& p1, const Line& p2, const Constraint& c);
Vector2 gradError(SketchEntity* e1, SketchEntity* e2, const Constraint* c);

class NewSketch {
public:
  NewSketch();
  ~NewSketch();

  std::shared_ptr<SketchEntity> findEntityById(std::shared_ptr<GeometricElement> v);
  std::shared_ptr<SketchEntity> findEntityByPosition(Vector2 pos, float threshold);
  float sgdStep();
  float totalError();
  void addPoint(std::shared_ptr<Point> p);
  void addLine(std::shared_ptr<Line> l);
  void addTrimmedLine(std::shared_ptr<TrimmedLine> l);
  void connect(
    std::shared_ptr<SketchEntity> a,
    std::shared_ptr<SketchEntity> b,
    std::shared_ptr<Constraint> c
  );
  void deleteEntity(std::shared_ptr<SketchEntity> a);
  void solve();

  std::vector<std::shared_ptr<SketchEntity>> entities;
  std::vector<std::shared_ptr<GuidedEntity>> guidedEntities;
private:
  std::vector<std::shared_ptr<Constraint>> edges;

  float stepSize = 0.02;
};

}

#endif
