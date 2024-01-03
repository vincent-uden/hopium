#include "Ui.h"
#include "raylib.h"
#include <cmath>

namespace Ui {

std::shared_ptr<Colorscheme> Ui::colorscheme = nullptr;

void Ui::setOnClick(std::function<void (Ui *)> f) {
  onClick = f;
}

void Ui::setOnMouseEnter(std::function<void (Ui *)> f) {
  onMouseEnter = f;
}

void Ui::setOnMouseExit(std::function<void (Ui *)> f) {
  onMouseExit = f;
}

Text::Text() {
    pos.x = 0;
    pos.y = 0;
    setText("");
    color = { 0, 0, 0, 255 };
}

Text::Text(std::string text) {
    pos.x = 0;
    pos.y = 0;
    setText(text);
    color = { 0, 0, 0, 255 };
}

Text::~Text() {
}

void Text::move(Vector2 distance) {
    pos.x += distance.x;
    pos.y += distance.y;
}

void Text::setPos(Vector2 pos) {
    Vector2 diff;
    diff.x = pos.x - this->pos.x;
    diff.y = pos.y - this->pos.y;
    move(diff);
}

void Text::draw() {
    DrawTextEx(colorscheme->font, text.c_str(), pos, 20, 1, color);
}

void Text::receiveMousePos(Vector2 mousePos) {
  if ( mousePos.x > pos.x && mousePos.x < pos.x + size.x ) {
    if ( mousePos.y > pos.y && mousePos.y < pos.y + size.y ) {
      // Mouse inside of bounds
      if (!hovered) {
        if (onMouseEnter) { onMouseEnter(this);
        }
      }
      hovered = true;
      return;
    }
  }

  if (hovered) {
    if (onMouseExit) {
      onMouseExit(this);
    }
  }

  hovered = false;
}

void Text::receiveMouseDown(Vector2 mousePos) {
}

void Text::receiveMouseUp(Vector2 mousePos) {
  if ( mousePos.x > pos.x && mousePos.x < pos.x + size.x ) {
    if ( mousePos.y > pos.y && mousePos.y < pos.y + size.y ) {
      if (onClick) {
        onClick(this);
      }
    }
  }
}

Vector2 Text::getSize() {
  return size;
}

void Text::setColor(Color c) {
  color = c;
}

void Text::setText(std::string text) {
  this->text = text;
  size = MeasureTextEx(GetFontDefault(), text.c_str(), 20, 1);
}

Rect::Rect() {
}

Rect::~Rect() {
}

void Rect::move(Vector2 distance) {
    bounds.x += distance.x;
    bounds.y += distance.y;
}

void Rect::setPos(Vector2 pos) {
    Vector2 diff;
    diff.x = pos.x - this->bounds.x;
    diff.y = pos.y - this->bounds.y;
    move(diff);
}

void Rect::draw() {
  DrawRectangleRec(bounds, bgColor);
}

void Rect::receiveMousePos(Vector2 mousePos) {
  if (CheckCollisionPointRec(mousePos, bounds)) {
    if (!hovered) {
      if (onMouseEnter) {
        onMouseEnter(this);
      }
    }
    hovered = true;
  } else {
    if (hovered) {
      if (onMouseExit) {
        onMouseExit(this);
      }
    }
    hovered = false;
  }
}

void Rect::receiveMouseDown(Vector2 mousePos) {
}

void Rect::receiveMouseUp(Vector2 mousePos) {
  if (contains(mousePos)) {
    if (onClick) {
      onClick(this);
    }
  }
}

Vector2 Rect::getSize() {
  return Vector2 { bounds.width, bounds.height };
}

void Rect::setColor(Color c) {
  bgColor = c;
}

void Rect::setSize(Vector2 size) {
  bounds.width = size.x;
  bounds.height = size.y;
}

bool Rect::contains(Vector2 mousePos) {
  return CheckCollisionPointRec(mousePos, bounds);
}

DropDown::DropDown(std::string label, std::vector<std::string> options) {
  this->options = options;
  btnSize = MeasureTextEx(GetFontDefault(), label.c_str(), 20, 1);
  btnSize.x += padding * 2;
  btnSize.y += padding * 2;

  size_t widest = 0;
  Vector2 currSize;
  Vector2 biggestSize = MeasureTextEx(GetFontDefault(), options[0].c_str(), 20, 1);
  for (size_t i = 0; i < options.size(); ++i) {
    currSize = MeasureTextEx(GetFontDefault(), options[i].c_str(), 20, 1);
    if (currSize.x > biggestSize.x) {
      biggestSize = currSize;
      widest = i;
    }
  }
  listSize = { biggestSize.x + padding * 2, (biggestSize.y + padding * 2) * options.size() };
  size_t i = 0;
  for (auto& opt : options) {
    std::shared_ptr<Text> uiOpt(new Text(opt));
    uiOpt->setPos({static_cast<float>(padding), (static_cast<float>(padding) * 2 + biggestSize.y) * (i+1) + padding});
    uiOpt->setColor({255, 255, 255, 255});

    std::shared_ptr<Rect> optBg(new Rect());
    optBg->setPos({0, (static_cast<float>(padding) * 2 + biggestSize.y) * (i+1)});
    optBg->setSize({ biggestSize.x + padding * 2, biggestSize.y + padding * 2 });
    optBg->setColor({30, 30, 30, 255});
    optBg->setOnMouseEnter([](Ui* p) {
        Rect* bg = static_cast<Rect*>(p);
        bg->setColor({155, 155, 155, 255});
    });
    optBg->setOnMouseExit([](Ui* p) {
        Rect* bg = static_cast<Rect*>(p);
        bg->setColor({30, 30, 30, 255});
    });
    optBg->setOnClick([this,i](Ui* p) {
        select(i);
    });

    ++i;
    uiOptions.push_back(uiOpt);
    uiOptionsBg.push_back(optBg);
  }

  this->label.setText(label);
  this->label.setColor({255, 255, 255, 255});
  this->label.setPos({ static_cast<float>(padding), static_cast<float>(padding)});

  this->label.setOnMouseEnter([](Ui* l) {
      Text* lbl = static_cast<Text*>(l);
      lbl->setColor({0, 255, 0, 255});
  });
  this->label.setOnMouseExit([](Ui* l) {
      Text* lbl = static_cast<Text*>(l);
      lbl->setColor({255, 255, 255, 255});
  });

  labelBg.setColor({255, 255, 255, 30});
  labelBg.setSize(btnSize);

  pos = {0, 0};
}

DropDown::~DropDown() {
}

void DropDown::move(Vector2 distance) {
  label.move(distance);
  for (auto& uiOpt : uiOptions) {
    uiOpt->move(distance);
  }
  labelBg.move(distance);
  for (auto& uiOptBg : uiOptionsBg) {
    uiOptBg->move(distance);
  }
  pos.x += distance.x;
  pos.y += distance.y;
}

void DropDown::setPos(Vector2 pos) {
    Vector2 diff;
    diff.x = pos.x - this->pos.x;
    diff.y = pos.y - this->pos.y;
    move(diff);
}

void DropDown::draw() {
  labelBg.draw();
  label.draw();
  if (open) {
    for (auto& optBg : uiOptionsBg) {
      optBg->draw();
    }
    for (auto& uiOpt : uiOptions) {
      uiOpt->draw();
    }
  }
}

void DropDown::receiveMousePos(Vector2 mousePos) {
  if (open) {
    if (
      CheckCollisionPointRec(mousePos, {pos.x, pos.y + btnSize.y, listSize.x, listSize.y}) ||
      CheckCollisionPointRec(mousePos, {pos.x, pos.y, btnSize.x, btnSize.y})
    ) {
      hovered = true;
    } else {
      hovered = false;
      open = false;
    }

    for (auto& optBg : uiOptionsBg) {
      optBg->receiveMousePos(mousePos);
    }
  } else {
    if (CheckCollisionPointRec(mousePos, {pos.x, pos.y, btnSize.x, btnSize.y})) {
      hovered = true;
      open = true;
    } else {
      hovered = false;
    }
  }
}

void DropDown::receiveMouseDown(Vector2 mousePos) {
  if (open) {
    if (
      CheckCollisionPointRec(mousePos, {pos.x, pos.y + btnSize.y, listSize.x, listSize.y})
    ) {
      for (auto& optBg : uiOptionsBg) {
        // If the onclick succeeds, we replace all UI with new UI and thus will
        // seg fault if we continue to loop over the old ui.
        if (optBg->contains(mousePos)) {
          optBg->receiveMouseDown(mousePos);
          break;
        }
      }
    }
  }
}

void DropDown::receiveMouseUp(Vector2 mousePos) {
  if (open) {
    if (
      CheckCollisionPointRec(mousePos, {pos.x, pos.y + btnSize.y, listSize.x, listSize.y})
    ) {
      for (auto& optBg : uiOptionsBg) {
        // If the onclick succeeds, we replace all UI with new UI and thus will
        // seg fault if we continue to loop over the old ui.
        if (optBg->contains(mousePos)) {
          optBg->receiveMouseUp(mousePos);
          break;
        }
      }
    }
  }
}

Vector2 DropDown::getSize() {
  if (open) {
    return listSize;
  } else {
    return btnSize;
  }
}

void DropDown::setOnSelected(std::function<void(std::string)> f) {
  onSelected = f;
}

void DropDown::select(size_t i) {
  if (onSelected) {
    onSelected(options[i]);
  }
}

Viewport::Viewport() {
  camera.position = { 20.0f, 20.0f, 20.0f };
  camera.target = { 0.0f, 0.0f, 0.0f };
  camera.up = { 0.0f, 1.0f, 0.0f };
  camera.fovy = 45.0f;
  camera.projection = CAMERA_PERSPECTIVE;
}

Viewport::~Viewport() {
}

void Viewport::move(Vector2 distance) {
}

void Viewport::setPos(Vector2 pos) {
}

void Viewport::draw() {
  BeginMode3D(camera);

  DrawLine3D(g0, g1, ApplicationState::getInstance()->holdingRotate ? GREEN : RED);
  DrawLine3D(g1, g2, ApplicationState::getInstance()->holdingRotate ? GREEN : RED);
  DrawLine3D(g2, g3, ApplicationState::getInstance()->holdingRotate ? GREEN : RED);
  DrawLine3D(g3, g0, ApplicationState::getInstance()->holdingRotate ? GREEN : RED);

  // DrawLine3D(lastRay.position, Vector3Add(lastRay.position, Vector3Scale(lastRay.direction, lastDist)), BLUE);

  if (scene) {
    for (size_t i = 0; i < scene->nBodies(); ++i) {
      scene->getBody(i)->draw();
    }

    for (size_t i = 0; i < scene->nPoints(); ++i) {
      std::shared_ptr<RasterVertex> v = scene->getPoint(i);
      if (hoveredId == v->id) {
        v->color = v->activeColor();
      } else {
        v->color = v->passiveColor();
      }
      v->draw();
    }

    for (size_t i = 0; i < scene->nShapes(); ++i) {
      scene->getShape(i)->draw();
      if (hoveredId == scene->getShape(i)->id) {
        scene->getShape(i)->color = scene->getShape(i)->activeColor();
      } else {
        scene->getShape(i)->color = scene->getShape(i)->passiveColor();
      }
    }
  }

  EndMode3D();
}

void Viewport::receiveMousePos(Vector2 mousePos) {
  if (ApplicationState::getInstance()->holdingRotate) {
    Vector2 diff = Vector2Subtract(mousePos, lastMousePos);

    double azimuthalDiff = diff.x * cameraSensetivity;
    double inclinationDiff = -diff.y * cameraSensetivity;

    double cameraAzimuthal = std::acos(camera.position.x / std::sqrt(camera.position.x * camera.position.x + camera.position.z * camera.position.z));
    if (camera.position.z < 0.0) {
      cameraAzimuthal = -cameraAzimuthal;
    }
    double cameraInclination = std::acos(camera.position.y / cameraRadius);

    camera.position.x = cameraRadius * std::cos(cameraAzimuthal + azimuthalDiff) * std::sin(cameraInclination + inclinationDiff);
    camera.position.z = cameraRadius * std::sin(cameraAzimuthal + azimuthalDiff) * std::sin(cameraInclination + inclinationDiff);
    camera.position.y = cameraRadius * std::cos(cameraInclination + inclinationDiff);
  }

  Ray ray = getNonOffsetMouseRay(mousePos);
  RayCollision groundHitInfo = GetRayCollisionQuad(ray, g0, g1, g2, g3);
  lastDist = groundHitInfo.distance;
  if (groundHitInfo.hit) {
    std::optional<size_t> maybeId = ApplicationState::getInstance()
      ->occtScene->idContainingPoint(
        groundHitInfo.point.x, groundHitInfo.point.y, groundHitInfo.point.z
      );
    if (maybeId.has_value()) {
      hoveredId = maybeId.value();
    } else  {
      hoveredId = -1;
    }
  }

  if (scene->nPoints() > 0) {
    std::optional<std::shared_ptr<RasterVertex>> maybeClosest = scene->queryVertex(ray, ApplicationState::getInstance()->selectionThreshold);

    if (maybeClosest.has_value()) {
      hoveredId = maybeClosest.value()->id;
    }
  }

  lastMousePos = mousePos;
}

void Viewport::receiveMouseDown(Vector2 mousePos) {
  RayCollision collision = { 0 };
  collision.distance = FLT_MAX;
  collision.hit = false;

  Ray ray = getNonOffsetMouseRay(mousePos);
  lastRay = ray;

  // Maybe we should attempt to raycast in the scene itself rather than the viewport?
  RayCollision groundHitInfo = GetRayCollisionQuad(ray, g0, g1, g2, g3);
  lastDist = groundHitInfo.distance;

  if ((groundHitInfo.hit && groundHitInfo.distance < collision.distance)) {
    EventQueue::getInstance()->postEvent(groundPlaneHit {
        groundHitInfo.point.x,
        groundHitInfo.point.y,
        groundHitInfo.point.z,
        ray
    });
  }
}

void Viewport::receiveMouseUp(Vector2 mousePos) {
}

Vector2 Viewport::getSize() {
  return Vector2 { areaSreenRect->width, areaSreenRect->height };
}

void Viewport::setScene(std::shared_ptr<Scene> scene) {
  this->scene = scene;
}

void Viewport::setAreaPointers(
  Rectangle* screenRect,
  Vector2* screenPos,
  RenderTexture* texture
) {
  areaSreenRect = screenRect;
  areaScreenPos = screenPos;
  areaTexture = texture;
}

Ray Viewport::getNonOffsetMouseRay(Vector2 mousePos) {
  Vector2 offset = { 0.0, 0.0 };
  if (areaSreenRect != nullptr && areaScreenPos != nullptr && areaTexture != nullptr) {
    offset.x = (areaSreenRect->width - areaTexture->texture.width) / 2.0;
    offset.y = (areaSreenRect->height - areaTexture->texture.height) / 2.0;
  }

  Ray ray = { 0 };
  ray = GetMouseRay(Vector2Subtract(mousePos, offset), camera);

  return ray;
}

ToolList::ToolList() {
  btnNames.push_back("Sketch");
  btnNames.push_back("Point");
  btnNames.push_back("Line");
  btnNames.push_back("Extrude");
  pos = { 0, 0 };

  btnSize = {80, 80};
  margin = 4;

  int i = 0;
  for (std::string& btnName : btnNames) {
    std::shared_ptr<Text> btnLbl(new Text(btnName));
    Vector2 textSize = MeasureTextEx(colorscheme->font, btnName.c_str(), 20, 1);
    btnLbl->setPos({(btnSize.x + margin) * i + (btnSize.x - textSize.x) / 2.0f, (btnSize.y - textSize.y) / 2.0f});
    btnLbl->setColor({255, 255, 255, 255});
    btnLbls.push_back(btnLbl);

    std::shared_ptr<Rect> btnBg(new Rect());
    btnBg->setSize(btnSize);
    btnBg->setColor(colorscheme->secondary);
    btnBg->setPos({(btnSize.x + margin) * i, 0});
    btnBg->setOnMouseEnter([](Ui* p) {
        Rect* bg = static_cast<Rect*>(p);
        bg->setColor(colorscheme->secondaryVariant);
    });
    btnBg->setOnMouseExit([](Ui* p) {
        Rect* bg = static_cast<Rect*>(p);
        bg->setColor(colorscheme->secondary);
    });
    btnBgs.push_back(btnBg);

    i++;
  }

  btnBgs[0]->setOnClick([](Ui* p) {
      EventQueue::getInstance()->postEvent(toggleSketchMode {});
  });
  btnBgs[1]->setOnClick([](Ui* p) {
      if (ApplicationState::getInstance()->sketchModeActive) {
        EventQueue::getInstance()->postEvent(togglePointMode {});
      }
  });
  btnBgs[2]->setOnClick([](Ui* p) {
      if (ApplicationState::getInstance()->sketchModeActive) {
        EventQueue::getInstance()->postEvent(toggleLineMode {});
      }
  });
  btnBgs[3]->setOnClick([](Ui* p) {
      if (ApplicationState::getInstance()->sketchModeActive) {
        EventQueue::getInstance()->postEvent(toggleExtrudeMode {});
      }
  });
}

ToolList::~ToolList() {
}

void ToolList::move(Vector2 distance) {
  for (auto& lbl : btnLbls) {
    lbl->move(distance);
  }
  for (auto& bg : btnBgs) {
    bg->move(distance);
  }
}

void ToolList::setPos(Vector2 pos) {
    Vector2 diff;
    diff.x = pos.x - this->pos.x;
    diff.y = pos.y - this->pos.y;
    move(diff);
}

void ToolList::draw() {
  int i = 0;
  for (auto& bg : btnBgs) {
    if (i == 0 || ApplicationState::getInstance()->sketchModeActive) {
      bg->draw();
    }
    ++i;
  }
  i = 0;
  for (auto& lbl : btnLbls) {
    if (i == 0) {
      lbl->draw();
    } else {
      ApplicationState* state = ApplicationState::getInstance();
      switch (i) {
      case 1:
        if (state->modeStack.isActive(state->point)) {
          lbl->setColor(colorscheme->active);
        } else {
          lbl->setColor(colorscheme->onBackground);
        }
        break;
      case 2:
        if (state->modeStack.isActive(state->line)) {
          lbl->setColor(colorscheme->active);
        } else {
          lbl->setColor(colorscheme->onBackground);
        }
        break;
      case 3:
        if (state->modeStack.isActive(state->extrude)) {
          lbl->setColor(colorscheme->active);
        } else {
          lbl->setColor(colorscheme->onBackground);
        }
        break;
      };
      if (ApplicationState::getInstance()->sketchModeActive) {
        lbl->draw();
      }
    }
    ++i;
  }
}

void ToolList::receiveMousePos(Vector2 mousePos) {
  int i = 0;
  for (auto& bg : btnBgs) {
    if (i == 0 || ApplicationState::getInstance()->sketchModeActive) {
      bg->receiveMousePos(mousePos);
    }
    ++i;
  }
  i = 0;
  for (auto& lbl : btnLbls) {
    if (i == 0 || ApplicationState::getInstance()->sketchModeActive) {
      lbl->receiveMousePos(mousePos);
    }
    ++i;
  }
}

void ToolList::receiveMouseDown(Vector2 mousePos) {
  for (auto& bg : btnBgs) {
    bg->receiveMouseDown(mousePos);
  }
  for (auto& lbl : btnLbls) {
    lbl->receiveMouseDown(mousePos);
  }
}

void ToolList::receiveMouseUp(Vector2 mousePos) {
  for (auto& bg : btnBgs) {
    bg->receiveMouseUp(mousePos);
  }
  for (auto& lbl : btnLbls) {
    lbl->receiveMouseUp(mousePos);
  }
}

Vector2 ToolList::getSize() {
  return Vector2 { (btnSize.x + margin) * btnNames.size() - margin, btnSize.y };
}

Icon::Icon() {
    pos.x = 0;
    pos.y = 0;
    loadedTexture = false;

    hoverTooltip = std::make_shared<Text>();
    hoverTooltip->setColor(WHITE);
    hoverTooltip->move(Vector2 { 4, 32 });
    hoverTooltipBg = std::make_shared<Rect>();
    hoverTooltipBg->setColor(colorscheme->secondaryVariant);
    hoverTooltipBg->move(Vector2 { 0, 32 });
    bg = std::make_shared<Rect>();
    bg->setColor(colorscheme->secondaryVariant);

    tooltipDelay = std::chrono::milliseconds(1000);
}

Icon::~Icon() {
  if (loadedTexture) {
    UnloadTexture(texture);
  }
}

void Icon::move(Vector2 distance) {
    pos.x += distance.x;
    pos.y += distance.y;

    hoverTooltip->move(distance);
    hoverTooltipBg->move(distance);
    bg->move(distance);
}

void Icon::setPos(Vector2 pos) {
    Vector2 diff;
    diff.x = pos.x - this->pos.x;
    diff.y = pos.y - this->pos.y;
    move(diff);
}

void Icon::draw() {
  if (hovered) {
    bg->draw();

  }
  DrawTexture(texture, static_cast<int>(pos.x), static_cast<int>(pos.y), WHITE);

  if (hovered && hoverBegin + tooltipDelay > ApplicationState::getInstance()->currentTime) {
    hoverTooltipBg->draw();
    hoverTooltip->draw();
  }
}

void Icon::receiveMousePos(Vector2 mousePos) {
  if ( mousePos.x > pos.x && mousePos.x < pos.x + size.x ) {
    if ( mousePos.y > pos.y && mousePos.y < pos.y + size.y ) {
      // Mouse inside of bounds
      if (!hovered) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
        hoverBegin = ApplicationState::getInstance()->currentTime;
        if (onMouseEnter) {
          onMouseEnter(this);
        }
      }
      hovered = true;
      return;
    }
  }

  if (hovered) {
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    if (onMouseExit) {
      onMouseExit(this);
    }
  }

  hovered = false;
}

void Icon::receiveMouseDown(Vector2 mousePos) {
}

void Icon::receiveMouseUp(Vector2 mousePos) {
  if ( mousePos.x > pos.x && mousePos.x < pos.x + size.x ) {
    if ( mousePos.y > pos.y && mousePos.y < pos.y + size.y ) {
      if (onClick) {
        onClick(this);
      }
    }
  }
}

Vector2 Icon::getSize() {
  return size;
}

void Icon::setImgPath(std::string path) {
  if (loadedTexture) {
    UnloadTexture(texture);
  }
  texture = LoadTexture(path.c_str());
  loadedTexture = true;
  size.x = texture.width;
  size.y = texture.height;
  bg->setSize(size);
}

void Icon::setHoverTooltip(std::string tooltip) {
  hoverTooltip->setText(tooltip);
  hoverTooltipBg->setSize(Vector2Add(hoverTooltip->getSize(), {12, 0}));
}

Row::Row() {
    pos.x = 0;
    pos.y = 0;
}

Row::~Row() {
  children.clear();
}

void Row::move(Vector2 distance) {
    pos.x += distance.x;
    pos.y += distance.y;

    for (const std::shared_ptr<Ui>& child: children) {
      child->move(distance);
    }
}

void Row::setPos(Vector2 pos) {
    Vector2 diff;
    diff.x = pos.x - this->pos.x;
    diff.y = pos.y - this->pos.y;
    move(diff);
}

void Row::draw() {
  for (const std::shared_ptr<Ui>& child: children) {
    child->draw();
  }
}

void Row::receiveMousePos(Vector2 mousePos) {
  for (const auto& child: children) {
    child->receiveMousePos(mousePos);
  }
  if ( mousePos.x > pos.x && mousePos.x < pos.x + size.x ) {
    if ( mousePos.y > pos.y && mousePos.y < pos.y + size.y ) {
      if (!hovered) {
        if (onMouseEnter) { onMouseEnter(this);
        }
      }
      hovered = true;
      return;
    }
  }

  if (hovered) {
    if (onMouseExit) {
      onMouseExit(this);
    }
  }

  hovered = false;
}

void Row::receiveMouseDown(Vector2 mousePos) {
    for (const auto& child: children) {
      child->receiveMouseDown(mousePos);
    }
}

void Row::receiveMouseUp(Vector2 mousePos) {
  if ( mousePos.x > pos.x && mousePos.x < pos.x + size.x ) {
    if ( mousePos.y > pos.y && mousePos.y < pos.y + size.y ) {
      if (onClick) {
        onClick(this);
      }

      for (const auto& child: children) {
        child->receiveMouseUp(mousePos);
      }
    }
  }
}

Vector2 Row::getSize() {
  return Vector2 { size.x, size.y };
}

void Row::addChild(std::shared_ptr<Ui> child) {
  if (children.size() == 0) {
    child->move(Vector2 { size.x, 0 });
    size = child->getSize();
  } else {
    child->move(Vector2 { size.x + margin, 0 });
    size.x += child->getSize().x + margin;
    size.y = std::max(size.y, child->getSize().y);
  }

  children.push_back(child);
}

GraphViewer::GraphViewer() {
  pos.x = 0;
  pos.y = 0;
  panOffset.x = 0;
  panOffset.y = 0;
}

GraphViewer::~GraphViewer() {
}

void GraphViewer::move(Vector2 distance) {
}

void GraphViewer::setPos(Vector2 pos) {
}

void GraphViewer::draw() {
  // Update positions, forces and velocities (Using verlet integration)
  for (size_t i = 0; i < graph->vertices.size(); ++i) {
    nodePos[i].x += nodeVel[i].x * dt + nodeAcc[i].x * dt * dt * 0.5;
    nodePos[i].y += nodeVel[i].y * dt + nodeAcc[i].y * dt * dt * 0.5;

    Vector2 a = Vector2Scale(nodeAcc[i], 0.5 * dt);
    nodeVel[i].x += a.x;
    nodeVel[i].y += a.y;

    nodeAcc[i].x = 0;
    nodeAcc[i].y = 0;
  }
  for (size_t i = 0; i < graph->vertices.size(); ++i) {
    Vector2 direction = Vector2Normalize(nodePos[i]);
    float mag = std::max(Vector2LengthSqr(nodePos[i]), 0.1f);
    nodeAcc[i].x -= direction.x / mag * centralAttraction;
    nodeAcc[i].y -= direction.y / mag * centralAttraction;

    for (size_t j = i + 1; j < graph->vertices.size(); ++j) {
      Vector2 diff = Vector2Subtract(nodePos[i], nodePos[j]);
      Vector2 f = Vector2Scale(Vector2Normalize(diff), 1.0f / Vector2LengthSqr(diff));
      nodeAcc[i].x += f.x * pushForce;
      nodeAcc[i].y += f.y * pushForce;
      nodeAcc[j].x -= f.x * pushForce;
      nodeAcc[j].y -= f.y * pushForce;

      if (graph->vertices[i]->isConnected(graph->vertices[j])) {
        float springL = Vector2Length(diff);
        nodeAcc[i].x -= diff.x * std::pow(springLength - springL, 2) * pullForce;
        nodeAcc[i].y -= diff.y * std::pow(springLength - springL, 2) * pullForce;
        nodeAcc[j].x += diff.x * std::pow(springLength - springL, 2) * pullForce;
        nodeAcc[j].y += diff.y * std::pow(springLength - springL, 2) * pullForce;
      }
    }
    Vector2 a = Vector2Scale(nodeAcc[i], 0.5 * dt);
    nodeVel[i].x += a.x;
    nodeVel[i].y += a.y;
    nodeVel[i].x *= velocityDamping;
    nodeVel[i].y *= velocityDamping;
  }
  if (grabbedId != -1) {
    Vector2 mousePosInWorldSpace = toGraphSpace(lastMousePos);
    nodePos[grabbedId].x = mousePosInWorldSpace.x;
    nodePos[grabbedId].y = mousePosInWorldSpace.y;
    nodeVel[grabbedId].x = 0;
    nodeVel[grabbedId].y = 0;
    nodeAcc[grabbedId].x = 0;
    nodeAcc[grabbedId].y = 0;
  }

  // Drawing
  size_t i = 0;
  for (const std::shared_ptr<GeometricElement>& v: graph->vertices) {
    Vector2 renderPos = toScreenSpace(nodePos[i]);
    DrawTextEx(
      colorscheme->font,
      v->label.c_str(),
      Vector2Add(renderPos, { -5, -30 }),
      20,
      1,
      colorscheme->onBackground
    );
    ++i;

    size_t j = 0;
    for (const std::shared_ptr<GeometricElement>& u: graph->vertices) {
      if (v->isConnected(u)) {
        DrawLineEx(
          renderPos,
          toScreenSpace(nodePos[j]),
          edgeThickness,
          {255, 255, 255, 50}
        );
      }
      ++j;
    }
  }

  i = 0;
  for (const std::shared_ptr<GeometricElement>& v: graph->vertices) {
    DrawCircleV(
      toScreenSpace(nodePos[i]),
      10,
      i == grabbedId ? Color {0, 255, 0, 255} : (i == hoveredId ? Color {0, 255, 0, 150} : Color {255, 255, 0, 150})
    );
    ++i;
  }
}

void GraphViewer::receiveMousePos(Vector2 mousePos) {
  lastMousePos = mousePos;

  hoveredId = -1;
  for (size_t i = 0; i < nodePos.size(); ++i) {
    if (Vector2Distance(toScreenSpace(nodePos[i]), mousePos) < selectThreshold) {
      hoveredId = i;
      break;
    }
  }
}

void GraphViewer::receiveMouseDown(Vector2 mousePos) {
  if (hoveredId != -1) {
    grabbedId = hoveredId;
  }
}

void GraphViewer::receiveMouseUp(Vector2 mousePos) {
  grabbedId = -1;
}

Vector2 GraphViewer::getSize() {
  return { areaScreenRect->width, areaScreenRect->height };
}

void GraphViewer::setAreaPointers(
  Rectangle* screenRect,
  Vector2* screenPos,
  RenderTexture* texture
) {
  areaScreenRect = screenRect;
  areaScreenPos = screenPos;
  areaTexture = texture;
}

void GraphViewer::setGraph(std::shared_ptr<ConstraintGraph> graph) {
  this->graph = graph;
  nodePos.clear();
  for (const std::shared_ptr<GeometricElement>& e: this->graph->vertices) {
    nodePos.push_back({ (std::rand() % 1000) / 1000.0f - 0.5f, (std::rand() % 1000) / 1000.0f - 0.5f });
    nodeVel.push_back({ 0.0, 0.0 });
    nodeAcc.push_back({ 0.0, 0.0 });
  }
}

Vector2 GraphViewer::toScreenSpace(const Vector2 p) {
  Vector2 out = p;
  out = Vector2Scale(out, scale);
  out = Vector2Add(out, Vector2Scale(getSize(), 0.5f));
  out = Vector2Add(pos, out);
  return out;
}

Vector2 GraphViewer::toGraphSpace(const Vector2 p) {
  Vector2 out = p;
  out = Vector2Subtract(out, Vector2Scale(getSize(), 0.5f));
  out = Vector2Subtract(out, pos);
  out = Vector2Scale(out, 1/scale);
  return out;
}

/* End of namespace */ }
