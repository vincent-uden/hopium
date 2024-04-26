#include "ApplicationState.h"

ApplicationState* ApplicationState::instance = nullptr;

ApplicationState* ApplicationState::getInstance() {
  if (instance == nullptr) {
    instance = new ApplicationState();
  }

  return instance;
}

ApplicationState::ApplicationState() {
}

ApplicationState::~ApplicationState() {
}


bool ApplicationState::active(std::shared_ptr<Sketch::SketchEntity> a) {
  return std::find(activeEntities.begin(), activeEntities.end(), a) != activeEntities.end();
}

void ApplicationState::addActive(std::shared_ptr<Sketch::SketchEntity> a) {
    a->active = true;
    activeEntities.push_back(a);
}
void ApplicationState::setActive(std::shared_ptr<Sketch::SketchEntity> a) {
    clearActive();
    addActive(a);
}
void ApplicationState::clearActive() {
    for (auto& e: activeEntities) {
        e->active = false;
    }
    activeEntities.clear();
}
