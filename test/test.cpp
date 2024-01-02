#include <BRepBuilderAPI_MakeFace.hxx>
#include <TopoDS_Wire.hxx>
#include <cctype>
#include <cmath>
#include <functional>
#include <gp_Pln.hxx>
#include <iostream>
#include <cassert>

#include <raylib.h>
#include <sstream>
#include <string>
#include <unistd.h>

#include "../src/Mode.h"
#include "../src/Renderer.h"
#include "../src/Ui.h"
#include "../src/OcctScene.h"
#include "../src/ConstraintGraph.h"

#define ADD_TEST(func, group) \
  tests.push_back({ func, #func, group });

#define ASSERT(expr, msg) \
  if (expr) { } else { std::cout << msg << std::endl; return(1); }

enum TestGroup {
  AREA_SPLITTING,
  MODES,
  GEOMETRY,
  HISTORY,
  CONSTRAINT_GRAPH,
};

typedef struct {
  std::function<int()> f;
  std::string name;
  TestGroup group;
} Test;

std::string prettifyFunctionName(std::string name) {
  std::stringstream ss;

  size_t i = 0;
  for (char c : name) {
    if (std::isupper(c)) {
      ss << " ";
    }
    if (i == 0) {
      c = std::toupper(c);
    }
    ss << c;
    ++i;
  }

  return ss.str();
}

void padToWidest(std::vector<Test>& tests) {
  int maxWidth = 0;
  for (size_t i = 0; i < tests.size(); ++i) {
    if (tests[i].name.size() > maxWidth) {
      maxWidth = tests[i].name.size();
    }
  }

  for (size_t i = 0; i < tests.size(); ++i) {
    size_t beforeLen = tests[i].name.size();
    for (size_t j = 0; j < maxWidth - beforeLen; ++j) {
      tests[i].name.push_back(' ');
    }
  }
}

void setupDecomposableTestGraph(ConstraintGraph& G) {
  std::shared_ptr<GeometricElement> a = std::make_shared<GeometricElement>(GeometricType::POINT, "a");
  std::shared_ptr<GeometricElement> b = std::make_shared<GeometricElement>(GeometricType::POINT, "b");
  std::shared_ptr<GeometricElement> c = std::make_shared<GeometricElement>(GeometricType::POINT, "c");
  std::shared_ptr<GeometricElement> d = std::make_shared<GeometricElement>(GeometricType::POINT, "d");
  std::shared_ptr<GeometricElement> e = std::make_shared<GeometricElement>(GeometricType::POINT, "e");
  std::shared_ptr<GeometricElement> f = std::make_shared<GeometricElement>(GeometricType::POINT, "f");
  std::shared_ptr<GeometricElement> g = std::make_shared<GeometricElement>(GeometricType::POINT, "g");
  std::shared_ptr<GeometricElement> h = std::make_shared<GeometricElement>(GeometricType::POINT, "h");

  std::shared_ptr<Constraint> ab = std::make_shared<Constraint>(ConstraintType::DISTANCE, "ab");
  std::shared_ptr<Constraint> bc = std::make_shared<Constraint>(ConstraintType::DISTANCE, "bc");
  std::shared_ptr<Constraint> ce = std::make_shared<Constraint>(ConstraintType::DISTANCE, "ce");
  std::shared_ptr<Constraint> ea = std::make_shared<Constraint>(ConstraintType::DISTANCE, "ea");
  std::shared_ptr<Constraint> cd = std::make_shared<Constraint>(ConstraintType::DISTANCE, "cd");
  std::shared_ptr<Constraint> ed = std::make_shared<Constraint>(ConstraintType::DISTANCE, "ed");
  std::shared_ptr<Constraint> ag = std::make_shared<Constraint>(ConstraintType::DISTANCE, "ag");
  std::shared_ptr<Constraint> af = std::make_shared<Constraint>(ConstraintType::DISTANCE, "af");
  std::shared_ptr<Constraint> fg = std::make_shared<Constraint>(ConstraintType::DISTANCE, "fg");
  std::shared_ptr<Constraint> fh = std::make_shared<Constraint>(ConstraintType::DISTANCE, "fh");
  std::shared_ptr<Constraint> gh = std::make_shared<Constraint>(ConstraintType::DISTANCE, "gh");
  std::shared_ptr<Constraint> df = std::make_shared<Constraint>(ConstraintType::DISTANCE, "df");
  std::shared_ptr<Constraint> dh = std::make_shared<Constraint>(ConstraintType::DISTANCE, "dh");

  G.addVertex(a);
  G.addVertex(b);
  G.addVertex(c);
  G.addVertex(d);
  G.addVertex(e);
  G.addVertex(f);
  G.addVertex(g);
  G.addVertex(h);

  G.connect(a, b, ab);
  G.connect(b, c, bc);
  G.connect(c, e, ce);
  G.connect(e, a, ea);
  G.connect(c, d, cd);
  G.connect(e, d, ed);
  G.connect(a, g, ag);
  G.connect(a, f, af);
  G.connect(f, g, fg);
  G.connect(f, h, fh);
  G.connect(g, h, gh);
  G.connect(d, f, df);
  G.connect(d, h, dh);
}

int windowCanBeSplitIntoArea() {
  const int screenWidth = 1600;
  const int screenHeight = 900;

  SetTraceLogLevel(LOG_NONE);
  InitWindow(screenWidth, screenHeight, "Raylib Example");

  SetTargetFPS(60);

  {
    Renderer renderer;
    renderer.init(screenWidth, screenHeight, ApplicationState::getInstance()->shaderStore);

    renderer.splitPaneVertical({ 10, 10 });

    ASSERT(renderer.areas.size() == 2, "Renderer should have 2 areas");
    ASSERT(renderer.boundaries.size() == 1, "Renderer should have 1 vertical boundary");

    Area* upper = renderer.areas[0].get();
    Area* lower = renderer.areas[1].get();
    Boundary* bdry = renderer.boundaries[0].get();

    ASSERT(upper->downBdry.get() == bdry, "The upper area should have the new boundary as it's down-facing boundary");
    ASSERT(lower->upBdry.get() == bdry, "The lower area should have the new boundary as it's up-facing boundary");
    ASSERT(bdry->side1[0].get() == upper, "The boundary should have the upper area as it's side1");
    ASSERT(bdry->side2[0].get() == lower, "The boundary should have the lower area as it's side2");
  }

  CloseWindow();

  return 0;
}

int windowCanBeSplitVerticallyRepeatedly() {
  const int screenWidth = 1600;
  const int screenHeight = 900;

  SetTraceLogLevel(LOG_NONE);
  InitWindow(screenWidth, screenHeight, "Raylib Example");

  SetTargetFPS(60);

  {
    Renderer renderer;
    renderer.init(screenWidth, screenHeight, ApplicationState::getInstance()->shaderStore);

    renderer.splitPaneVertical({ 10, 10 });
    renderer.splitPaneVertical({ 10, 10 });

    ASSERT(renderer.areas.size() == 3, "Renderer should have 2 areas");
    ASSERT(renderer.boundaries.size() == 2, "Renderer should have 1 vertical boundary");

    Area* upper = renderer.areas[0].get();
    Area* lower = renderer.areas[1].get();
    Area* middle = renderer.areas[2].get();
    // Since we split the initial area twice, the second boundary is situated
    // above the first.
    Boundary* bdryUpper = renderer.boundaries[1].get();
    Boundary* bdryLower = renderer.boundaries[0].get();

    ASSERT(bdryUpper->side1.size() == 1 && bdryUpper->side2.size() == 1, "The upper boundary should only have one neighbour on each side");
    ASSERT(bdryLower->side1.size() == 1 && bdryLower->side2.size() == 1, "The lower boundary should only have one neighbour on each side");

    ASSERT(upper->downBdry.get() == bdryUpper && !upper->upBdry && !upper->leftBdry && !upper->rightBdry, "The upper area should only have one adjacent boundary below");
    ASSERT(lower->upBdry.get() == bdryLower && !lower->downBdry && !lower->leftBdry && !lower->rightBdry, "The lower area should only have one adjacent boundary above");
    ASSERT(middle->upBdry.get() == bdryUpper && middle->downBdry.get() == bdryLower && !middle->leftBdry && !middle->rightBdry, "The middle area should be enclosed by the upper and lower boundary");
  }

  CloseWindow();

  return 0;
}

int windowCanBeSplitHorizontallyRepeatedly() {
  const int screenWidth = 1600;
  const int screenHeight = 900;

  SetTraceLogLevel(LOG_NONE);
  InitWindow(screenWidth, screenHeight, "Raylib Example");

  SetTargetFPS(60);

  {
    Renderer renderer;
    renderer.init(screenWidth, screenHeight, ApplicationState::getInstance()->shaderStore);

    renderer.splitPaneHorizontal({ 10, 10 });
    renderer.splitPaneHorizontal({ 10, 10 });

    ASSERT(renderer.areas.size() == 3, "Renderer should have 2 areas");
    ASSERT(renderer.boundaries.size() == 2, "Renderer should have 1 vertical boundary");

    Area* left = renderer.areas[0].get();
    Area* right = renderer.areas[1].get();
    Area* middle = renderer.areas[2].get();
    // Since we split the initial area twice, the second boundary is situated
    // above the first.
    Boundary* bdryLeft = renderer.boundaries[1].get();
    Boundary* bdryRight = renderer.boundaries[0].get();

    ASSERT(bdryLeft->side1.size() == 1 && bdryLeft->side2.size() == 1, "The left boundary should only have one neighbour on each side");
    ASSERT(bdryRight->side1.size() == 1 && bdryRight->side2.size() == 1, "The right boundary should only have one neighbour on each side");

    ASSERT(left->rightBdry.get() == bdryLeft && !left->upBdry && !left->leftBdry && !left->downBdry, "The left area should only have one adjacent boundary to the right");
    ASSERT(right->leftBdry.get() == bdryRight && !right->downBdry && !right->upBdry && !right->rightBdry, "The right area should only have one adjacent boundary to the left");
    ASSERT(middle->leftBdry.get() == bdryLeft && middle->rightBdry.get() == bdryRight && !middle->downBdry && !middle->upBdry, "The middle area should be enclosed by the left and right boundaries");
  }

  CloseWindow();

  return 0;
}

bool areClose(double a, double b) {
  return std::fabs(a - b) < 0.001f;
}

int deepRecursiveSplittingProducesCorrectSizes() {
  const int screenWidth = 1600;
  const int screenHeight = 900;

  SetTraceLogLevel(LOG_NONE);
  InitWindow(screenWidth, screenHeight, "Raylib Example");

  SetTargetFPS(60);

  {
    Renderer renderer;
    renderer.init(screenWidth, screenHeight, ApplicationState::getInstance()->shaderStore);

    renderer.splitPaneVertical({ 10, 10 });
    renderer.splitPaneHorizontal({ 10, 600 });
    renderer.splitPaneVertical({ 10, 600 });
    renderer.splitPaneHorizontal({ 10, 800 });

    Boundary* horizontal1 = renderer.boundaries[0].get();
    Boundary* vertical1 = renderer.boundaries[1].get();
    Boundary* horizontal2 = renderer.boundaries[2].get();
    Boundary* vertical2 = renderer.boundaries[3].get();

    ASSERT(areClose(horizontal1->extent() / 2.0f, horizontal2->extent()), "The smaller horizontal boundary should be half the length of the big one");
    ASSERT(areClose(vertical1->extent() / 2.0f, vertical2->extent()), "The smaller vertical boundary should be half the length of the big one");
  }

  CloseWindow();

  return 0;
}

int serializeAndDeserializeRenderer() {
  const int screenWidth = 1600;
  const int screenHeight = 900;

  SetTraceLogLevel(LOG_NONE);
  InitWindow(screenWidth, screenHeight, "Raylib Example");

  SetTargetFPS(60);

  {
    Renderer renderer;
    renderer.init(screenWidth, screenHeight, ApplicationState::getInstance()->shaderStore);

    renderer.splitPaneVertical({ 10, 10 });
    renderer.splitPaneHorizontal({ 10, 600 });
    renderer.splitPaneVertical({ 10, 600 });
    renderer.splitPaneHorizontal({ 10, 800 });

    json serialized = renderer.serialize();
    renderer.deserialize(serialized);

    ASSERT(renderer.areas.size() == 5, "Renderer should have the 5 areas it had previously");
    ASSERT(renderer.boundaries.size() == 4, "Renderer should have the 4 boundaries it had previously");
  }

  CloseWindow();

  return 0;
}

int canSplitAndCollapseBoundary() {
  const int screenWidth = 1600;
  const int screenHeight = 900;

  SetTraceLogLevel(LOG_NONE);
  InitWindow(screenWidth, screenHeight, "Raylib Example");

  SetTargetFPS(60);

  {
    Renderer renderer;
    renderer.init(screenWidth, screenHeight, ApplicationState::getInstance()->shaderStore);

    renderer.splitPaneHorizontal({ 10, 10 });
    renderer.collapseBoundary({ 800, 450 });

    ASSERT(renderer.areas.size() == 1, "Renderer should have 1 area. Actually has: " << renderer.areas.size());
    ASSERT(renderer.boundaries.size() == 0, "Renderer should have 0 boundaries");
    ASSERT(renderer.areas[0]->screenRect.width == 1600 && renderer.areas[0]->screenRect.height == 900, "Area should reclaim initially occupied space");
  }

  CloseWindow();

  return 0;
}

int collapsingBoundaryReconnectsNowAdjacentAreas() {
  const int screenWidth = 1600;
  const int screenHeight = 900;

  SetTraceLogLevel(LOG_NONE);
  InitWindow(screenWidth, screenHeight, "Raylib Example");

  SetTargetFPS(60);

  {
    Renderer renderer;
    renderer.init(screenWidth, screenHeight, ApplicationState::getInstance()->shaderStore);

    renderer.splitPaneVertical({ 10, 10 });
    renderer.splitPaneVertical({ 10, 10 });
    renderer.collapseBoundary({ 800, screenHeight / 4.0 });

    ASSERT(renderer.boundaries.size() == 1, "There should only be one boundary");
    ASSERT(renderer.areas.size() == 2, "There should only be two areas");
    ASSERT(renderer.boundaries[0]->side1.size() == 1, "The first boundary should once again be connected to the first area");
    ASSERT(renderer.boundaries[0]->side1[0].get() == renderer.areas[0].get(), "The first boundary should once again be connected to the boundary");
  }

  CloseWindow();

  return 0;
}

int nestedSplitAndCollapseDoesntBreakGraph() {
  const int screenWidth = 1600;
  const int screenHeight = 900;

  SetTraceLogLevel(LOG_NONE);
  InitWindow(screenWidth, screenHeight, "Raylib Example");

  SetTargetFPS(60);

  {
    Renderer renderer;
    renderer.init(screenWidth, screenHeight, ApplicationState::getInstance()->shaderStore);

    renderer.splitPaneHorizontal({ 10, 10 });
    renderer.splitPaneHorizontal({ 10, 10 });
    renderer.collapseBoundary({ screenWidth / 4.0, screenHeight / 2.0 });
    renderer.splitPaneVertical({ 10, 10 });

    Area* upLeft = renderer.areas[0].get();
    Area* right = renderer.areas[1].get();
    Area* downLeft = renderer.areas[2].get();

    Boundary* vertical = renderer.boundaries[0].get();
    Boundary* horizontal = renderer.boundaries[1].get();

    ASSERT(renderer.boundaries.size() == 2, "There should only be two boundaries");
    ASSERT(renderer.areas.size() == 3, "There should only be three areas");

    ASSERT(upLeft->isLeftOf(vertical), "Up left area should be left of the vertical boundary");
    ASSERT(upLeft->isAbove(horizontal), "Up left area should be above the horizontal boundary");
    ASSERT(downLeft->isLeftOf(vertical), "Down left area should be left of the vertical boundary");
    ASSERT(downLeft->isBelow(horizontal), "Down left area should be below the horizontal boundary");
    ASSERT(right->isRightOf(vertical), "Right area should be right of the vertical boundary");
  }

  CloseWindow();

  return 0;
}

int modeStackManipulatesCorrectly() {
  class TestMode : public Mode {
  public:
    TestMode() {
    }

    bool keyPress(KeyPress key) {
      return true;
    }

    bool keyRelease(KeyPress key) {
      return true;
    }

    bool mousePress(MouseKeyPress button) {
      return true;
    }

    bool mouseRelease(MouseKeyPress button) {
      return true;
    }
  };
  std::shared_ptr<Mode> mode1(new TestMode());
  std::shared_ptr<Mode> mode2(new TestMode());
  std::shared_ptr<Mode> mode3(new TestMode());
  std::shared_ptr<Mode> mode4(new TestMode());

  ModeStack modeStack;
  modeStack.push(mode1);
  modeStack.push(mode2);
  modeStack.push(mode3);
  modeStack.push(mode4);

  modeStack.exit(mode2);

  ASSERT(modeStack.size() == 1, "Mode stack should only have 1 element");
  ASSERT(modeStack.peek(0) == mode1, "Mode 1 should be on top of the stack ");

  return 0;
}

int wireFixProducesWorkingWire() {
  OcctScene scene;

  gp_Pnt p1(0.0, 0.0, 0.0);
  gp_Pnt p2(0.1, 0.0, 0.0);
  gp_Pnt p3(0.0, 0.1, 0.0);

  Handle(Geom_TrimmedCurve) c1 = GC_MakeSegment(p1, p2);
  Handle(Geom_TrimmedCurve) c2 = GC_MakeSegment(p1, p3);

  TopoDS_Edge e1 = BRepBuilderAPI_MakeEdge(c1);
  TopoDS_Edge e2 = BRepBuilderAPI_MakeEdge(c2);

  TopoDS_Wire w1 = BRepBuilderAPI_MakeWire(e1, e2).Wire();

  gp_Pln plane;
  TopoDS_Face f = BRepBuilderAPI_MakeFace(plane);

  ShapeAnalysis_Wire checkWire;
  checkWire.SetPrecision(1.e-3);
  checkWire.Load(w1);
  checkWire.SetFace(f);

  ASSERT(checkWire.IsLoaded(), "Checker should be loaded");
  ASSERT(checkWire.IsReady(), "Checker should be ready");
  ASSERT(checkWire.CheckOrder(), "Wire should not be ordered");
  ASSERT(!checkWire.CheckConnected(), "Wire should be connected");
  ASSERT(!checkWire.CheckSmall(), "Wire should not contain any small edges");

  ShapeFix_Wire fixWire;
  fixWire.Load(w1);
  fixWire.SetFace(f);
  fixWire.Perform();

  w1 = fixWire.Wire();
  checkWire.Load(w1);

  TopoDS_Face f2 = BRepBuilderAPI_MakeFace(w1).Face();
  ASSERT(!f2.IsNull(), "F2 should've been created");

  ASSERT(!checkWire.CheckOrder(), "Wire should be ordered");
  ASSERT(!checkWire.CheckConnected(), "Wire should be connected");
  ASSERT(!checkWire.CheckSmall(), "Wire should not contain any small edges");

  return 0;
}

int serializeAndDeserializeEventHistoryProducesUnity() {
  class TestEventQueue: public EventQueue {
  public:
    std::vector<AppEvent> getHistory() { return history; }
  };
  TestEventQueue queue;

  queue.postEvent(enableSketchMode {});
  queue.postEvent(togglePointMode {});
  queue.postEvent(groundPlaneHit { 1.0, 0.0, 0.0, Ray { Vector3 { 1.0, 0.0, 0.0 }, Vector3 { 2.0, 0.0, 0.0 } }});
  queue.postEvent(groundPlaneHit { 0.0, 2.0, 0.0, Ray { Vector3 { -1.0, 0.0, 0.0 }, Vector3 { 1.0, 0.0, 0.0 } }});
  queue.postEvent(groundPlaneHit { 0.0, 0.0, 3.0, Ray { Vector3 { 2.0, 0.0, 0.0 }, Vector3 { -1.0, 0.0, 0.0 } }});

  std::vector<AppEvent> beforeSerialization = queue.getHistory();
  size_t histLen = beforeSerialization.size();

  json serialized = queue.serializeHistory();

  queue.deserializeHistory(serialized);
  std::vector<AppEvent> afterSerialization = queue.getHistory();

  ASSERT(beforeSerialization.size() == histLen, "We should have a clone of the history");
  ASSERT(beforeSerialization.size() == afterSerialization.size(), "There should be as many events after serialzation as there were before");

  for (size_t i = 0; i < beforeSerialization.size(); i++) {
    ASSERT(beforeSerialization[i].index() == afterSerialization[i].index(), "The events should be the same");

    if (std::holds_alternative<groundPlaneHit>(beforeSerialization[i])) {
      groundPlaneHit hit1 = std::get<groundPlaneHit>(beforeSerialization[i]);
      groundPlaneHit hit2 = std::get<groundPlaneHit>(afterSerialization[i]);

      ASSERT(
        hit1.x == hit2.x && hit1.y == hit2.y && hit1.z == hit2.z &&
        hit1.ray.position.x == hit2.ray.position.x && hit1.ray.position.y == hit2.ray.position.y && hit1.ray.position.z == hit2.ray.position.z &&
        hit1.ray.direction.x == hit2.ray.direction.x && hit1.ray.direction.y == hit2.ray.direction.y && hit1.ray.direction.z == hit2.ray.direction.z,
        "Rays should be identical"
      );
    }
  }

  return 0;
}

int breadthFirstSearchProducesShortestPath() {
  ConstraintGraph G;

  std::shared_ptr<GeometricElement> e0 = std::make_shared<GeometricElement>(GeometricType::POINT, "e0");
  std::shared_ptr<GeometricElement> e1 = std::make_shared<GeometricElement>(GeometricType::POINT, "e1");
  std::shared_ptr<GeometricElement> e2 = std::make_shared<GeometricElement>(GeometricType::POINT, "e2");
  std::shared_ptr<GeometricElement> e3 = std::make_shared<GeometricElement>(GeometricType::POINT, "e3");
  std::shared_ptr<GeometricElement> e4 = std::make_shared<GeometricElement>(GeometricType::POINT, "e4");
  std::shared_ptr<GeometricElement> e5 = std::make_shared<GeometricElement>(GeometricType::POINT, "e5");

  std::shared_ptr<Constraint> c12 = std::make_shared<Constraint>(ConstraintType::DISTANCE, "c12");
  std::shared_ptr<Constraint> c01 = std::make_shared<Constraint>(ConstraintType::DISTANCE, "c01");
  std::shared_ptr<Constraint> c03 = std::make_shared<Constraint>(ConstraintType::DISTANCE, "c03");
  std::shared_ptr<Constraint> c04 = std::make_shared<Constraint>(ConstraintType::DISTANCE, "c04");
  std::shared_ptr<Constraint> c35 = std::make_shared<Constraint>(ConstraintType::DISTANCE, "c35");
  std::shared_ptr<Constraint> c45 = std::make_shared<Constraint>(ConstraintType::DISTANCE, "c45");

  G.addVertex(e0);
  G.addVertex(e1);
  G.addVertex(e2);
  G.addVertex(e3);
  G.addVertex(e4);
  G.addVertex(e5);

  G.connect(e2, e1, c12);
  G.connect(e0, e1, c01);
  G.connect(e0, e3, c03);
  G.connect(e0, e4, c04);
  G.connect(e3, e5, c35);
  G.connect(e4, e5, c45);

  std::optional<std::vector<std::shared_ptr<Constraint>>> shortestPath = G.breadthFirstSearch(e2, e5);
  ASSERT(shortestPath.has_value(), "There should be a path");

  std::vector<std::shared_ptr<Constraint>> path = shortestPath.value();
  ASSERT(path.size() == 4, "There should be 4 constraints in the path");

  std::vector<std::shared_ptr<Constraint>> expectedPath = { c12, c01, c03, c35 };
  for (size_t i = 0; i < path.size() - 1; ++i) {
    ASSERT(expectedPath[i] == path[i], "The constraints should be the same");
  }

  shortestPath = G.breadthFirstSearch(e4, e1);
  ASSERT(shortestPath.has_value(), "There should be a path");
  path = shortestPath.value();
  expectedPath = { c04, c01 };
  for (size_t i = 0; i < path.size() - 1; ++i) {
    ASSERT(expectedPath[i] == path[i], "The constraints should be the same");
  }

  return 0;
}

int maxFlowAlgorithmProducesCorrectSolution() {
  ConstraintGraph G;

  std::shared_ptr<GeometricElement> e0 = std::make_shared<GeometricElement>(GeometricType::POINT, "e0");
  std::shared_ptr<GeometricElement> e1 = std::make_shared<GeometricElement>(GeometricType::POINT, "e1");
  std::shared_ptr<GeometricElement> e2 = std::make_shared<GeometricElement>(GeometricType::POINT, "e2");
  std::shared_ptr<GeometricElement> e3 = std::make_shared<GeometricElement>(GeometricType::POINT, "e3");
  std::shared_ptr<GeometricElement> e4 = std::make_shared<GeometricElement>(GeometricType::POINT, "e4");
  std::shared_ptr<GeometricElement> e5 = std::make_shared<GeometricElement>(GeometricType::POINT, "e5");

  std::shared_ptr<Constraint> c12 = std::make_shared<Constraint>(ConstraintType::DISTANCE, "c12");
  std::shared_ptr<Constraint> c01 = std::make_shared<Constraint>(ConstraintType::DISTANCE, "c01");
  std::shared_ptr<Constraint> c03 = std::make_shared<Constraint>(ConstraintType::DISTANCE, "c03");
  std::shared_ptr<Constraint> c04 = std::make_shared<Constraint>(ConstraintType::DISTANCE, "c04");
  std::shared_ptr<Constraint> c35 = std::make_shared<Constraint>(ConstraintType::DISTANCE, "c35");
  std::shared_ptr<Constraint> c45 = std::make_shared<Constraint>(ConstraintType::DISTANCE, "c45");

  G.addVertex(e0);
  G.addVertex(e1);
  G.addVertex(e2);
  G.addVertex(e3);
  G.addVertex(e4);
  G.addVertex(e5);

  G.connect(e2, e1, c12);
  G.connect(e0, e1, c01);
  G.connect(e0, e3, c03);
  G.connect(e0, e4, c04);
  G.connect(e3, e5, c35);
  G.connect(e4, e5, c45);

  ASSERT(1 == G.maxFlow(e2, e0), "Flow should be 1 from 2 -> 0");
  ASSERT(2 == G.maxFlow(e0, e5), "Flow should be 2 from 0 -> 5");
  ASSERT(2 == G.maxFlow(e5, e0), "Flow should be 2 from 5 -> 0");
  ASSERT(1 == G.maxFlow(e2, e5), "Flow should be 1 from 2 -> 5");

  return 0;
}

int canCorrectlyClassifyTriconnectedGraph() {
  std::shared_ptr<GeometricElement> e0 = std::make_shared<GeometricElement>(GeometricType::POINT, "e0");
  std::shared_ptr<GeometricElement> e1 = std::make_shared<GeometricElement>(GeometricType::POINT, "e1");
  std::shared_ptr<GeometricElement> e2 = std::make_shared<GeometricElement>(GeometricType::POINT, "e2");
  std::shared_ptr<GeometricElement> e3 = std::make_shared<GeometricElement>(GeometricType::POINT, "e3");

  std::shared_ptr<Constraint> c01 = std::make_shared<Constraint>(ConstraintType::DISTANCE, "c01");
  std::shared_ptr<Constraint> c12 = std::make_shared<Constraint>(ConstraintType::DISTANCE, "c12");
  std::shared_ptr<Constraint> c23 = std::make_shared<Constraint>(ConstraintType::DISTANCE, "c23");
  std::shared_ptr<Constraint> c30 = std::make_shared<Constraint>(ConstraintType::DISTANCE, "c30");
  std::shared_ptr<Constraint> c02 = std::make_shared<Constraint>(ConstraintType::DISTANCE, "c02");
  std::shared_ptr<Constraint> c13 = std::make_shared<Constraint>(ConstraintType::DISTANCE, "c13");

  ConstraintGraph G;
  G.addVertex(e0);
  G.addVertex(e1);
  G.addVertex(e2);
  G.addVertex(e3);

  G.connect(e0, e1, c01);
  G.connect(e1, e2, c12);
  ASSERT(!G.connected(), "Graph shouldn't be connected");
  G.connect(e2, e3, c23);
  G.connect(e3, e0, c30);
  G.connect(e0, e2, c02);

  ASSERT(G.connected(), "Graph should be connected");

  ASSERT(!G.triconnected(), "Graph shouldn't be triconnected yet");

  G.connect(e1, e3, c13);
  ASSERT(G.triconnected(), "Graph should be triconnected");

  std::shared_ptr<GeometricElement> e4 = std::make_shared<GeometricElement>(GeometricType::POINT, "e4");
  std::shared_ptr<Constraint> c14 = std::make_shared<Constraint>(ConstraintType::DISTANCE, "c14");
  G.addVertex(e4);
  G.connect(e1, e4, c14);
  ASSERT(!G.triconnected(), "Graph shouldn't be triconnected now");

  return 0;
}

int canIdentifySeparatingVertices() {
  // Create points up from a,b ... to h
  ConstraintGraph G;
  setupDecomposableTestGraph(G);

  ASSERT(!G.triconnected(), "This graph shouldn't be triconnected");

  auto pair = G.separatingVertices();
  ASSERT(pair.first && pair.second, "The separation should succeed");

  G.deleteVertex(pair.first);
  G.deleteVertex(pair.second);

  ASSERT(!G.connected(), "G shouldn't be connected");

  return 0;
}

int graphDeepCopyProducesIdenticalGraph() {
  ConstraintGraph G;
  setupDecomposableTestGraph(G);

  std::shared_ptr<ConstraintGraph> G2 = G.deepCopy();

  ASSERT(G.vertices.size() == G2->vertices.size(), "The number of vertices should be the same");
  for (size_t i = 0; i < G.vertices.size(); ++i) {
    ASSERT(G.vertices[i]->label == G2->vertices[i]->label && G.vertices[i].get() != G2->vertices[i].get(), "The vertices should be the same");
    ASSERT(G.vertices[i]->edges.size() == G2->vertices[i]->edges.size(), "There should be the same number of edges");
  }
  ASSERT(G.edges.size() == G2->edges.size(), "There should be the same number of edges");

  return 0;
}

int canSeparateGraphIntoConnectedComponents() {
  ConstraintGraph G;
  setupDecomposableTestGraph(G);

  std::pair<std::shared_ptr<GeometricElement>, std::shared_ptr<GeometricElement>> p = G.separatingVertices();
  std::pair<std::shared_ptr<ConstraintGraph>, std::shared_ptr<ConstraintGraph>> GPrimes = G.separatingGraphs(p.first, p.second);
  ASSERT(GPrimes.first->connected() && GPrimes.second->connected(), "The graphs should be connected");
  ASSERT(GPrimes.first->vertices.size() == 7 && GPrimes.second->vertices.size() == 3, "The number of vertices should be 7 and 3");

  return 0;
}

typedef struct {
  std::optional<TestGroup> group;
} CliArgs ;

std::optional<TestGroup> group(std::string name) {
  if (name == "AREA_SPLITTING") {
    return std::optional(TestGroup::AREA_SPLITTING);
  } else if (name == "MODES") {
    return std::optional(TestGroup::MODES);
  } else if (name == "GEOMETRY") {
    return std::optional(TestGroup::GEOMETRY);
  } else if (name== "HISTORY") {
    return std::optional(TestGroup::HISTORY);
  } else if (name == "CONSTRAINT_GRAPH") {
    return std::optional(TestGroup::CONSTRAINT_GRAPH);
  }

  return std::nullopt;
}

CliArgs parseCliArgs(int argc, char** argv) {
  CliArgs args;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--group") == 0 || strcmp(argv[i], "-g") == 0) {
      args.group = group(std::string(argv[++i]));
    }
  }

  return args;
}

int main(int argc, char** argv) {
  std::vector<Test> tests;

  ADD_TEST(windowCanBeSplitIntoArea, AREA_SPLITTING);
  ADD_TEST(windowCanBeSplitVerticallyRepeatedly, AREA_SPLITTING);
  ADD_TEST(windowCanBeSplitHorizontallyRepeatedly, AREA_SPLITTING);
  ADD_TEST(deepRecursiveSplittingProducesCorrectSizes, AREA_SPLITTING);
  ADD_TEST(serializeAndDeserializeRenderer, AREA_SPLITTING);
  ADD_TEST(canSplitAndCollapseBoundary, AREA_SPLITTING);
  ADD_TEST(collapsingBoundaryReconnectsNowAdjacentAreas, AREA_SPLITTING);
  ADD_TEST(nestedSplitAndCollapseDoesntBreakGraph, AREA_SPLITTING);
  ADD_TEST(modeStackManipulatesCorrectly, MODES);
  ADD_TEST(wireFixProducesWorkingWire, GEOMETRY);
  ADD_TEST(serializeAndDeserializeEventHistoryProducesUnity, HISTORY);
  ADD_TEST(breadthFirstSearchProducesShortestPath, CONSTRAINT_GRAPH);
  ADD_TEST(maxFlowAlgorithmProducesCorrectSolution, CONSTRAINT_GRAPH);
  ADD_TEST(canCorrectlyClassifyTriconnectedGraph, CONSTRAINT_GRAPH);
  ADD_TEST(canIdentifySeparatingVertices, CONSTRAINT_GRAPH);
  ADD_TEST(graphDeepCopyProducesIdenticalGraph, CONSTRAINT_GRAPH);
  ADD_TEST(canSeparateGraphIntoConnectedComponents, CONSTRAINT_GRAPH);

  for (auto& test : tests) {
    test.name = prettifyFunctionName(test.name);
  }
  padToWidest(tests);

  CliArgs args = parseCliArgs(argc, argv);
  std::cout << "Running Tests" << std::endl;
  std::cout << "=========================" << std::endl;
  int passed = 0;
  int total = 0;
  for (auto test : tests) {
    if (!args.group.has_value() || args.group.value() == test.group) {
      std::cout << test.name << " | ";
      int result = test.f();
      printf("%c[%dm", 0x1B, result == 0 ? 32 : 31);
      std::cout  << (result == 0 ? "PASSED" : "FAILED") << std::endl;
      printf("%c[%dm", 0x1B, 0);
      if (result == 0) {
        ++passed;
      }
      ++total;
    }
  }
  std::cout << passed << "/" << total << " tests passed" << std::endl;;
  if (tests.size() - passed > 0) {
    std::cout << total - passed << " tests failed" << std::endl;
  }
}
