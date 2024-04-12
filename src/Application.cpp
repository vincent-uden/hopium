#include "Application.h"

Application* Application::instance = nullptr;

Application* Application::getInstance() {
  if (instance == nullptr) {
    instance = new Application();
  }

  return instance;
}

void Application::update() {
  state->modeStack.update();
  state->currentTime = std::chrono::system_clock::now();

  Vector2 mousePos = GetMousePosition();
  renderer.receiveMousePos(mousePos);
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    renderer.mouseDown(mousePos);
  }
  if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
    renderer.mouseUp(mousePos);
  }
  renderer.mouseWheelMove(GetMouseWheelMove());

  while (!eventQueue.empty()) {
    AppEvent event = eventQueue.pop();
    std::visit([this](auto&& arg){ processEvent(arg); }, event);
    state->modeStack.processEvent(event);
  }

  renderer.draw();
}

Application::Application() {
  state = ApplicationState::getInstance();

  layoutPath = "layout.json";
  scenePath = "history.json";
  renderer.init(1600, 900, state->shaderStore);

  createBottle();
  buildGraph2();
  std::srand(1337);

  int count = GetMonitorCount();
  if (count == 4) {
    SetWindowPosition(1920 + (1920 - 1600)/2, (1080 - 900) / 2);
  }

  if (fileExists(layoutPath)) {
    json layout = json::parse(readFromFile(layoutPath));
    renderer.deserialize(layout);
  }

  state->global = std::shared_ptr<Mode>(new GlobalMode());
  state->modeStack.push(state->global);

  state->sketch = std::shared_ptr<Mode>(new SketchMode());
  state->point = std::shared_ptr<Mode>(new PointMode());
  state->line = std::shared_ptr<Mode>(new LineMode());
  state->tline = std::shared_ptr<Mode>(new TLineMode());
  state->extrude = std::shared_ptr<Mode>(new ExtrudeMode());


  if (fileExists(scenePath)) {
    json history = json::parse(readFromFile(scenePath));
    eventQueue.deserializeHistory(history);
    eventQueue.resetHistoryIndex();
    for (size_t i = 0; i < eventQueue.historySize(); ++i) {
      std::optional<AppEvent> nextEvent = eventQueue.getNextHistoryEvent();
      if (nextEvent.has_value()) {
        std::visit([this](auto&& arg){ processEvent(arg); }, nextEvent.value());
      }
    }
    state->scene->setShapes(state->occtScene->rasterizeShapes());
  }
}

void Application::buildGraph() {
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

  state->graph = std::make_shared<ConstraintGraph>();

  state->graph->addVertex(a);
  state->graph->addVertex(b);
  state->graph->addVertex(c);
  state->graph->addVertex(d);
  state->graph->addVertex(e);
  state->graph->addVertex(f);
  state->graph->addVertex(g);
  state->graph->addVertex(h);

  state->graph->connect(a, b, ab);
  state->graph->connect(b, c, bc);
  state->graph->connect(c, e, ce);
  state->graph->connect(e, a, ea);
  state->graph->connect(c, d, cd);
  state->graph->connect(e, d, ed);
  state->graph->connect(a, g, ag);
  state->graph->connect(a, f, af);
  state->graph->connect(f, g, fg);
  state->graph->connect(f, h, fh);
  state->graph->connect(g, h, gh);
  state->graph->connect(d, f, df);
  state->graph->connect(d, h, dh);

  state->stree = analyze(state->graph);
}

void Application::buildGraph2() {
  std::shared_ptr<GeometricElement> a = std::make_shared<GeometricElement>(GeometricType::POINT, "a");
  std::shared_ptr<GeometricElement> b = std::make_shared<GeometricElement>(GeometricType::POINT, "b");
  std::shared_ptr<GeometricElement> c = std::make_shared<GeometricElement>(GeometricType::POINT, "c");
  std::shared_ptr<GeometricElement> d = std::make_shared<GeometricElement>(GeometricType::POINT, "d");
  std::shared_ptr<GeometricElement> e = std::make_shared<GeometricElement>(GeometricType::POINT, "e");

  std::shared_ptr<Constraint> ab  = std::make_shared<Constraint>(ConstraintType::VERTICAL, "ab");
  std::shared_ptr<Constraint> ab2 = std::make_shared<Constraint>(ConstraintType::DISTANCE, "ab2");
  std::shared_ptr<Constraint> bc  = std::make_shared<Constraint>(ConstraintType::HORIZONTAL, "bc");
  std::shared_ptr<Constraint> bd  = std::make_shared<Constraint>(ConstraintType::DISTANCE, "bd");
  std::shared_ptr<Constraint> ac  = std::make_shared<Constraint>(ConstraintType::DISTANCE, "ac");
  std::shared_ptr<Constraint> ce  = std::make_shared<Constraint>(ConstraintType::HORIZONTAL, "ce");
  std::shared_ptr<Constraint> cd  = std::make_shared<Constraint>(ConstraintType::VERTICAL, "cd");
  std::shared_ptr<Constraint> de  = std::make_shared<Constraint>(ConstraintType::DISTANCE, "de");
  ab2->value = 3;
  ac->value = 5;
  de->value = std::sqrt(2.0f);
  bd->value = 6;

  state->graph = std::make_shared<ConstraintGraph>();

  state->graph->addVertex(a);
  state->graph->addVertex(b);
  state->graph->addVertex(c);
  state->graph->addVertex(d);
  state->graph->addVertex(e);
  state->graph->connect(a, b, ab);
  state->graph->connect(a, b, ab2);
  state->graph->connect(b, c, bc);
  state->graph->connect(b, d, bd);
  state->graph->connect(a, c, ac);
  state->graph->connect(c, e, ce);
  state->graph->connect(c, d, cd);
  state->graph->connect(d, e, de);

  state->stree = analyze(state->graph);
}

void Application::buildSketch() {
  state->paramSketch = std::make_shared<Sketch::NewSketch>();
  std::shared_ptr<GeometricElement> a = std::make_shared<GeometricElement>(GeometricType::POINT, "a");
  std::shared_ptr<GeometricElement> b = std::make_shared<GeometricElement>(GeometricType::POINT, "b");
  std::shared_ptr<GeometricElement> c = std::make_shared<GeometricElement>(GeometricType::POINT, "c");
  std::shared_ptr<Sketch::Point> pa = std::make_shared<Sketch::Point>(a);
  pa->fixed = true;
  std::shared_ptr<Sketch::Point> pb = std::make_shared<Sketch::Point>(b);
  std::shared_ptr<Sketch::Point> pc = std::make_shared<Sketch::Point>(c);
  pa->pos = { 0.0, 0.0 };
  pb->pos = { 0.2, 1.0 };
  pc->pos = { 1.0, 0.2 };
  state->paramSketch->addPoint(pa);
  state->paramSketch->addPoint(pb);
  state->paramSketch->addPoint(pc);
  std::shared_ptr<Constraint> ab  = std::make_shared<Constraint>(ConstraintType::VERTICAL, "ab");
  std::shared_ptr<Constraint> ac  = std::make_shared<Constraint>(ConstraintType::HORIZONTAL, "ac");
  std::shared_ptr<Constraint> ac2  = std::make_shared<Constraint>(ConstraintType::DISTANCE, "ac2");
  std::shared_ptr<Constraint> ab2  = std::make_shared<Constraint>(ConstraintType::DISTANCE, "ab2");
  ac2->value = 5.0;
  ab2->value = 3.0;
  state->paramSketch->connect(pa, pb, ab);
  state->paramSketch->connect(pa, pb, ab2);
  state->paramSketch->connect(pa, pc, ac);
  state->paramSketch->connect(pa, pc, ac2);

  std::shared_ptr<GeometricElement> d = std::make_shared<GeometricElement>(GeometricType::LINE, "d");
  std::shared_ptr<Sketch::Line> pd = std::make_shared<Sketch::Line>(d);
  pd->k = 1.0;
  pd->m = 0.0;
  state->paramSketch->addLine(pd);
  std::shared_ptr<Constraint> ad  = std::make_shared<Constraint>(ConstraintType::COINCIDENT, "ad");
  std::shared_ptr<Constraint> cd  = std::make_shared<Constraint>(ConstraintType::COINCIDENT, "cd");
  state->paramSketch->connect(pa, pd, ad);
  state->paramSketch->connect(pc, pd, cd);

  std::shared_ptr<GeometricElement> e = std::make_shared<GeometricElement>(GeometricType::LINE, "e");
  std::shared_ptr<Sketch::Line> pe = std::make_shared<Sketch::Line>(e);
  pd->k = -10.0;
  pd->m = 5.0;
  state->paramSketch->addLine(pe);
  std::shared_ptr<Constraint> ce  = std::make_shared<Constraint>(ConstraintType::DISTANCE, "ce");
  ce->value = 3.0;
  std::shared_ptr<Constraint> be  = std::make_shared<Constraint>(ConstraintType::COINCIDENT, "be");
  state->paramSketch->connect(pc, pe, ce);
  state->paramSketch->connect(pb, pe, be);
}

void Application::processEvent(enableSketchMode event) {
}

void Application::processEvent(disableSketchMode event) {
}

void Application::processEvent(toggleSketchMode event) {
}

void Application::processEvent(popMode event) {
}

void Application::processEvent(togglePointMode event) {
}

void Application::processEvent(toggleLineMode event) {
}

void Application::processEvent(toggleTLineMode event) {
}

void Application::processEvent(toggleExtrudeMode event) {
}

void Application::processEvent(startRotate event) {
}

void Application::processEvent(stopRotate event) {
}

void Application::processEvent(splitPaneHorizontally event) {
    renderer.splitPaneHorizontal(event.mousePos);
}

void Application::processEvent(splitPaneVertically event) {
    renderer.splitPaneVertical(event.mousePos);
}

void Application::processEvent(collapseBoundary event) {
    renderer.collapseBoundary(event.mousePos);
}

void Application::processEvent(dumpShapes event) {
}

void Application::processEvent(exitProgram event) {
  shouldExit = true;
}

void Application::processEvent(sketchPlaneHit event) {
}

void Application::processEvent(sketchClick event) {
}

void Application::processEvent(sketchConstrain event) {
}

void Application::processEvent(increaseZoom event) {
}

void Application::processEvent(decreaseZoom event) {
}

Application::~Application() {
  writeToFile(layoutPath, renderer.serialize().dump(-1));
  writeToFile(scenePath, eventQueue.serializeHistory().dump(-1));
}
