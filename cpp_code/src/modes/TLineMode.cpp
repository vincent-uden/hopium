#include "TLineMode.h"

TLineMode::TLineMode() {
}

TLineMode::~TLineMode() {
}

bool TLineMode::processEvent(AppEvent event) {
  ApplicationState* state = ApplicationState::getInstance();
  if (toggleTLineMode* e = std::get_if<toggleTLineMode>(&event)) {
    state->modeStack.exit(state->tline);
    return true;
  } else if (sketchClick* e = std::get_if<sketchClick>(&event)) {
    Vector2 mousePos(e->x, e->y);
    state->activeCoordinates.push_back(mousePos);
    if (state->activeCoordinates.size() == 2) {
      std::shared_ptr<GeometricElement> eStart
        = std::make_shared<GeometricElement>(GeometricType::POINT, "");
      std::shared_ptr<GeometricElement> eEnd
        = std::make_shared<GeometricElement>(GeometricType::POINT, "");
      std::shared_ptr<GeometricElement> eLine
        = std::make_shared<GeometricElement>(GeometricType::LINE, "");
      std::shared_ptr<Sketch::Point> start = std::make_shared<Sketch::Point>(eStart);
      std::shared_ptr<Sketch::Point> end = std::make_shared<Sketch::Point>(eEnd);
      std::shared_ptr<Sketch::Line> line = std::make_shared<Sketch::Line>(eLine);
      Vector2 c0 = state->activeCoordinates[0];
      Vector2 c1 = state->activeCoordinates[1];
      start->pos = c0;
      start->draw = false;
      end->pos = c1;
      end->draw = false;
      line->k = (c1.y-c0.y)/(c1.x-c0.x);
      line->m = -c0.x * line->k + c0.y;
      line->draw = false;
      std::shared_ptr<Sketch::TrimmedLine> tLine =
        std::make_shared<Sketch::TrimmedLine>(start, end, line);
      state->paramSketch->addPoint(start);
      state->paramSketch->addPoint(end);
      state->paramSketch->addLine(line);
      state->paramSketch->addTrimmedLine(tLine);
      state->activeCoordinates.clear();
      EventQueue::getInstance()->postEvent(popMode {});
      return true;
    }
  }
  // TODO: Extrude Occt scene

  return false;
}

bool TLineMode::keyPress(KeyPress key) {
  bool consumed = true;

  switch (key.key) {
  case KEY_ESCAPE:
    EventQueue::getInstance()->postEvent(popMode {});
    break;
  case KEY_L:
    break;
  default:
    consumed = false;
  }

  return consumed;
}

bool TLineMode::keyRelease(KeyPress key) {
  bool consumed = true;

  switch (key.key) {
  default:
    consumed = false;
  }

  return consumed;
}

bool TLineMode::mousePress(MouseKeyPress button) {
  bool consumed = false;

  return consumed;
}

bool TLineMode::mouseRelease(MouseKeyPress button) {
  bool consumed = true;
  switch (button.button) {
  case MOUSE_BUTTON_LEFT:
    break;
  default:
    consumed = false;
  }

  return false;
}

