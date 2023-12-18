#ifndef UDEN_APP_STATE
#define UDEN_APP_STATE

#include <memory>

#include <gp_Pnt.hxx>

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
  std::shared_ptr<Mode> line;
  std::shared_ptr<Mode> extrude;

  // Name subject to change. Active doesn't really descibe the usage well. This
  // is for actions requiring multiple points such as drawing a line or arc.
  std::vector<gp_Pnt> activePoints;

  double selectionThreshold = 0.6;

  std::chrono::time_point<std::chrono::system_clock> currentTime;

private:
  ApplicationState();
  static ApplicationState* instance;
};

#endif
