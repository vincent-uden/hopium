#include "Application.h"

Application* Application::instance = nullptr;

Application* Application::getInstance() {
  if (instance == nullptr) {
    instance = new Application();
  }

  return instance;
}

void Application::postEvent(AppEvent event) {
  eventQueue.push(event);
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

  renderer.draw();
}

Application::Application(): renderer(1600, 900) {
  layoutPath = "layout.json";

  createBottle();

  int count = GetMonitorCount();
  if (count == 4) {
    SetWindowPosition(1920 + (1920 - 1600)/2, (1080 - 900) / 2);
  }

  if (fileExists(layoutPath)) {
    json layout = json::parse(readFromFile(layoutPath));
    renderer.deserialize(layout);
  }

  std::shared_ptr<Mode> global(new GlobalMode(&renderer));
  modeStack.push(global);
}

Application::~Application() {
  writeToFile(layoutPath, renderer.serialize().dump(-1));
}
