#ifndef UDEN_APP_STATE
#define UDEN_APP_STATE

#include <memory>

#include "Mode.h"
#include "OcctScene.h"
#include "Scene.h"

class ApplicationState {
public:
  static ApplicationState* getInstance();
  ~ApplicationState();

  std::shared_ptr<ShaderStore> shaderStore = std::make_shared<ShaderStore>();
  std::shared_ptr<Scene> scene = std::shared_ptr<Scene>(new Scene(shaderStore));;
  std::shared_ptr<OcctScene> occtScene = std::shared_ptr<OcctScene>(new OcctScene());;

  bool holdingRotate = false;
  bool sketchModeActive = false;

  ModeStack modeStack;

  std::shared_ptr<Mode> global;
  std::shared_ptr<Mode> sketch;
  std::shared_ptr<Mode> point;

private:
  ApplicationState();
  static ApplicationState* instance;
};

#endif
