#ifndef UDEN_APP_STATE
#define UDEN_APP_STATE

#include <memory>

#include <gp_Pnt.hxx>

#include "Mode.h"
#include "rendering/Scene.h"
#include "cad/ConstraintGraph.h"
#include "cad/OcctScene.h"
#include "cad/Sketch.h"

class ApplicationState {
public:
  static ApplicationState* getInstance();
  ~ApplicationState();

  std::shared_ptr<ConstraintGraph> graph = std::shared_ptr<ConstraintGraph>(new ConstraintGraph());;
  std::shared_ptr<STree> stree = std::shared_ptr<STree>(new STree());
  std::shared_ptr<Sketch::NewSketch> paramSketch = std::make_shared<Sketch::NewSketch>();
  std::shared_ptr<ShaderStore> shaderStore = std::make_shared<ShaderStore>();
  std::shared_ptr<Scene> scene = std::shared_ptr<Scene>(new Scene(shaderStore));;
  std::shared_ptr<ParametricScene> occtScene = std::shared_ptr<ParametricScene>(new ParametricScene());;

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
  double zoom = 1.0;

  std::chrono::time_point<std::chrono::system_clock> currentTime;

  // TODO: Start at -1, only active once sketch is edited
  int editingSketchId = 0;

private:
  ApplicationState();
  static ApplicationState* instance;
};

#endif
