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

#define ADD_TEST(func) \
  tests.push_back({ func, #func});

#define ASSERT(expr, msg) \
  if (expr) { } else { std::cout << msg << std::endl; return(1); }

typedef struct {
  std::function<int()> f;
  std::string name;
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

int main(int argc, char** argv) {
  std::vector<Test> tests;

  ADD_TEST(windowCanBeSplitIntoArea);
  ADD_TEST(windowCanBeSplitVerticallyRepeatedly);
  ADD_TEST(windowCanBeSplitHorizontallyRepeatedly);
  ADD_TEST(deepRecursiveSplittingProducesCorrectSizes);
  ADD_TEST(serializeAndDeserializeRenderer);
  ADD_TEST(canSplitAndCollapseBoundary);
  ADD_TEST(collapsingBoundaryReconnectsNowAdjacentAreas);
  ADD_TEST(nestedSplitAndCollapseDoesntBreakGraph);
  ADD_TEST(modeStackManipulatesCorrectly);
  ADD_TEST(wireFixProducesWorkingWire);
  ADD_TEST(serializeAndDeserializeEventHistoryProducesUnity);

  for (auto& test : tests) {
    test.name = prettifyFunctionName(test.name);
  }
  padToWidest(tests);

  std::cout << "Running Tests" << std::endl;
  std::cout << "=========================" << std::endl;
  int passed = 0;
  for (auto test : tests) {
    std::cout << test.name << " | ";
    int result = test.f();
    printf("%c[%dm", 0x1B, result == 0 ? 32 : 31);
    std::cout  << (result == 0 ? "PASSED" : "FAILED") << std::endl;
    printf("%c[%dm", 0x1B, 0);
    if (result == 0) {
      ++passed;
    }
  }
  std::cout << passed << "/" << tests.size() << " tests passed" << std::endl;;
  if (tests.size() - passed > 0) {
    std::cout << tests.size() - passed << " tests failed" << std::endl;
  }
}
