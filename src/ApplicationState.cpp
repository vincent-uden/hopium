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
