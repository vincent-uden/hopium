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

NewSketch::NewSketch() {
}

NewSketch::~NewSketch() {
}

std::shared_ptr<SketchEntity> NewSketch::findEntityById(std::shared_ptr<GeometricElement> v) {
  for (const auto& p: entities) {
    if (p->v->id == v->id) {
      return p;
    }
  }

  return nullptr;
}

std::shared_ptr<SketchEntity> NewSketch::findEntityByPosition(Vector2 pos, float threshold) {
  Point fakePoint(nullptr);
  fakePoint.pos = pos;

  Constraint fakeC(ConstraintType::DISTANCE);
  fakeC.value = 0.0;

  std::shared_ptr<SketchEntity> closest = nullptr;
  float closestDist = INFINITY;

  for (const auto& p: entities) {
    float dist = error(&fakePoint, p.get(), &fakeC);
    if (dist < closestDist) {
      closest = p;
      closestDist = dist;
    }
  }
  if (closestDist > threshold) {
    closest = nullptr;
  }

  return closest;
}

float NewSketch::sgdStep() {
  std::vector<Vector2> step;
  float err = 0.0f;
  for (const auto& p: entities) {
    step.push_back({0.0f, 0.0f});
  }

  int total_constraints = 0;
  int i = 0;
  for (const auto& p1: entities) {
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
  for (auto& p: entities) {
    p->update(Vector2Scale(step[i], stepSize));
    ++i;
  }

  return err;
}

float NewSketch::totalError() {
  float total = 0.0f;
  for (const auto& p: entities) {
    for (const auto& [edge, other] : p->v->edges) {
      const auto p2 = findEntityById(other);
      total += error(p.get(), p2.get(), edge.get());
    }
  }

  return total / 2.0f;
}

void NewSketch::addPoint(std::shared_ptr<Point> p) {
  entities.push_back(p);
}

void NewSketch::addLine(std::shared_ptr<Line> l) {
  entities.push_back(l);
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
  std::erase_if(entities, [a](std::shared_ptr<SketchEntity> b) { return a->v->id == b->v->id; });
  for (const auto& [edge, other] : a->v->edges) {
    other->deleteEdge(a->v);
    std::erase_if(edges, [edge](std::shared_ptr<Constraint> c) { return c == edge; });
  }
}

}
