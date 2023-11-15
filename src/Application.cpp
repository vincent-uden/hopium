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

void Application::processEvent(enableSketchMode event) {
  std::cout << "sketch mode enabled" << std::endl;
}

void Application::processEvent(disableSketchMode event) {
  std::cout << "sketch mode disabled" << std::endl;
}

void Application::processEvent(toggleSketchMode event) {
  std::cout << "sketch mode toggled" << std::endl;
}

Application::~Application() {
  writeToFile(layoutPath, renderer.serialize().dump(-1));
}
