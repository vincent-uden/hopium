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

  while (!eventQueue.empty()) {
    AppEvent event = eventQueue.pop();
    std::visit([this](auto&& arg){ processEvent(arg); }, event);
  }

  renderer.draw();
}

Application::Application() {
  state = ApplicationState::getInstance();

  layoutPath = "layout.json";
  scenePath = "history.json";
  renderer.init(1600, 900, state->shaderStore);

  createBottle();
  buildGraph();
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

void Application::processEvent(enableSketchMode event) {
  state->modeStack.push(state->sketch);
  state->sketchModeActive = true;
}

void Application::processEvent(disableSketchMode event) {
  state->modeStack.exit(state->sketch);
  state->sketchModeActive = false;
}

void Application::processEvent(toggleSketchMode event) {
  if (state->modeStack.isActive(state->sketch)) {
    state->modeStack.exit(state->sketch);
    state->sketchModeActive = false;
  } else {
    state->modeStack.push(state->sketch);
    state->sketchModeActive = true;
  }
}

void Application::processEvent(popMode event) {
  if (state->modeStack.size() > 1) {
    state->modeStack.pop();
  }

  state->sketchModeActive = state->modeStack.isActive(state->sketch);
}

void Application::processEvent(togglePointMode event) {
  if (state->modeStack.isActive(state->point)) {
    state->modeStack.exit(state->point);
  } else {
    if (state->modeStack.isInnerMostMode(state->sketch)) {
      state->modeStack.push(state->point);
    }
  }
}

void Application::processEvent(toggleLineMode event) {
  if (state->modeStack.isActive(state->line)) {
    state->modeStack.exit(state->line);
  } else {
    if (state->modeStack.isInnerMostMode(state->sketch)) {
      state->modeStack.push(state->line);
    }
  }
}

void Application::processEvent(toggleExtrudeMode event) {
  if (state->modeStack.isActive(state->extrude)) {
    state->modeStack.exit(state->extrude);
  } else {
    if (state->modeStack.isInnerMostMode(state->sketch)) {
      state->modeStack.push(state->extrude);
    }
  }
}

void Application::processEvent(startRotate event) {
  state->holdingRotate = true;
}

void Application::processEvent(stopRotate event) {
  state->holdingRotate = false;
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
  state->occtScene->dumpShapes();
}

void Application::processEvent(exitProgram event) {
  std::cout << "EXITING" << std::endl;
  shouldExit = true;
}

void Application::processEvent(groundPlaneHit event) {
  if (state->modeStack.isActive(state->point)) {
    state->occtScene->createPoint(event.x, event.y, event.z);
    state->scene->setPoints(state->occtScene->rasterizePoints());
  } else if (state->modeStack.isActive(state->line)) {
    std::optional<std::shared_ptr<RasterVertex>> p = state->scene->queryVertex(event.ray, state->selectionThreshold);
    if (p.has_value()) {
      event.x = p.value()->x;
      event.y = p.value()->y;
      event.z = p.value()->z;
    }
    if (state->activePoints.size() < 1) {
      state->activePoints.push_back(gp_Pnt(event.x, event.y, event.z));
    } else {
      state->activePoints.push_back(gp_Pnt(event.x, event.y, event.z));
      state->occtScene->createLine(state->activePoints[0], state->activePoints[1], 1e-5);
      state->scene->setShapes(state->occtScene->rasterizeShapes());
      state->activePoints.clear();
      EventQueue::getInstance()->postEvent(toggleLineMode {});
    }
  } else if (state->modeStack.isActive(state->extrude)) {
    std::optional<size_t> maybeId = ApplicationState::getInstance()
      ->occtScene->idContainingPoint(event.x, event.y, event.z);

    if (maybeId.has_value()) {
      state->occtScene->extrude(maybeId.value(), 0.5);
      state->scene->setShapes(state->occtScene->rasterizeShapes());
    }
  }
}

void Application::processEvent(increaseZoom event) {
  state->zoom *= 1.25;
}

void Application::processEvent(decreaseZoom event) {
  state->zoom /= 1.25;
}

Application::~Application() {
  writeToFile(layoutPath, renderer.serialize().dump(-1));
  writeToFile(scenePath, eventQueue.serializeHistory().dump(-1));
}
