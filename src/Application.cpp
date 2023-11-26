#include "Application.h"

Application* Application::instance = nullptr;

Application* Application::getInstance() {
  if (instance == nullptr) {
    instance = new Application();
  }

  return instance;
}

void Application::update() {
  modeStack.update();

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
    // Visit is the type-safe way to call a function on a variant
    std::visit([this](auto&& arg){ processEvent(arg); }, event);
  }

  renderer.draw();
}

Application::Application() {
  state = ApplicationState::getInstance();
  std::cout << "Application initialized: " << ApplicationState::getInstance() << std::endl;

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

  global = std::shared_ptr<Mode>(new GlobalMode(&renderer));
  modeStack.push(global);

  sketch = std::shared_ptr<Mode>(new SketchMode());
  point = std::shared_ptr<Mode>(new PointMode());

  state->scene->addBodyFromFile("../assets/toilet_rolls.obj");
  state->scene->addBodyFromFile("../assets/toilet_rolls.obj");
  state->scene->getBody(1)->pos.x = 1.0 * 5;
  state->scene->addBodyFromFile("../assets/toilet_rolls.obj");
  state->scene->getBody(2)->pos.x = -1.0 * 5;
  state->scene->addBodyFromFile("../assets/toilet_rolls.obj");
  state->scene->getBody(3)->pos.x = 2.0 * 5;
}

void Application::processEvent(enableSketchMode event) {
  modeStack.push(sketch);
}

void Application::processEvent(disableSketchMode event) {
  modeStack.exit(sketch);
}

void Application::processEvent(toggleSketchMode event) {
  if (modeStack.isActive(sketch)) {
    modeStack.exit(sketch);
  } else {
    modeStack.push(sketch);
  }
}

void Application::processEvent(popMode event) {
  if (modeStack.size() > 1) {
    modeStack.pop();
  }
}

void Application::processEvent(togglePointMode event) {
  if (modeStack.isActive(point)) {
    modeStack.exit(point);
  } else {
    modeStack.push(point);
  }
}

void Application::processEvent(startRotate event) {
  state->holdingRotate = true;
}

void Application::processEvent(stopRotate event) {
  state->holdingRotate = false;
}

void Application::processEvent(exitProgram event) {
  shouldExit = true;
}

void Application::processEvent(groundPlaneHit event) {
  if (modeStack.isActive(point)) {
    state->occtScene->createPoint(event.x, event.y, event.z);
    state->scene->setPoints(state->occtScene->rasterizePoints());
  }
}

Application::~Application() {
  writeToFile(layoutPath, renderer.serialize().dump(-1));
}
