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
