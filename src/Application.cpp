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
  renderer.init(1600, 900, state->shaderStore);

  createBottle();

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

  state->scene->addBodyFromFile("../assets/toilet_rolls.obj"); state->scene->addBodyFromFile("../assets/toilet_rolls.obj");
  state->scene->getBody(1)->pos.x = 1.0 * 5;
  state->scene->addBodyFromFile("../assets/toilet_rolls.obj");
  state->scene->getBody(2)->pos.x = -1.0 * 5;
  state->scene->addBodyFromFile("../assets/toilet_rolls.obj");
  state->scene->getBody(3)->pos.x = 2.0 * 5;
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
    state->modeStack.push(state->point);
  }
}

void Application::processEvent(toggleLineMode event) {
  if (state->modeStack.isActive(state->line)) {
    state->modeStack.exit(state->line);
  } else {
    state->modeStack.push(state->line);
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
  shouldExit = true;
}

void Application::processEvent(groundPlaneHit event) {
  if (state->modeStack.isActive(state->point)) {
    state->occtScene->createPoint(event.x, event.y, event.z);
    state->scene->setPoints(state->occtScene->rasterizePoints());
  }
  if (state->modeStack.isActive(state->line)) {
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
  }
}

Application::~Application() {
  writeToFile(layoutPath, renderer.serialize().dump(-1));
}
