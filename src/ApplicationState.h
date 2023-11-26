#ifndef UDEN_APP_STATE
#define UDEN_APP_STATE

#include <memory>

#include "Scene.h"

class ApplicationState {
public:
  static ApplicationState* getInstance();
  ~ApplicationState();

  std::shared_ptr<Scene> scene;
  std::shared_ptr<ShaderStore> shaderStore = std::make_shared<ShaderStore>();

private:
  ApplicationState();
  static ApplicationState* instance;
};

#endif
