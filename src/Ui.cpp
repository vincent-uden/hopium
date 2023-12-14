#include "Ui.h"

std::shared_ptr<Colorscheme> Ui::colorscheme = nullptr;

UiText::UiText() {
    pos.x = 0;
    pos.y = 0;
    setText("");
    color = { 0, 0, 0, 255 };
}

UiText::UiText(std::string text) {
    pos.x = 0;
    pos.y = 0;
    setText(text);
    color = { 0, 0, 0, 255 };
}

UiText::~UiText() {
}

void UiText::move(Vector2 distance) {
    pos.x += distance.x;
    pos.y += distance.y;
}

void UiText::setPos(Vector2 pos) {
    Vector2 diff;
    diff.x = pos.x - this->pos.x;
    diff.y = pos.y - this->pos.y;
    move(diff);
}

void UiText::draw() {
    DrawTextEx(colorscheme->font, text.c_str(), pos, 20, 1, color);
}

void UiText::receiveMousePos(Vector2 mousePos) {
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

void UiText::receiveMouseDown(Vector2 mousePos) {
}

void UiText::receiveMouseUp(Vector2 mousePos) {
  if ( mousePos.x > pos.x && mousePos.x < pos.x + size.x ) {
    if ( mousePos.y > pos.y && mousePos.y < pos.y + size.y ) {
      if (onClick) {
        onClick(this);
      }
    }
  }
}

void Ui::setOnClick(std::function<void (Ui *)> f) {
  onClick = f;
}

void Ui::setOnMouseEnter(std::function<void (Ui *)> f) {
  onMouseEnter = f;
}

void Ui::setOnMouseExit(std::function<void (Ui *)> f) {
  onMouseExit = f;
}

void UiText::setColor(Color c) {
  color = c;
}

void UiText::setText(std::string text) {
  this->text = text;
  size = MeasureTextEx(GetFontDefault(), text.c_str(), 20, 1);
}

UiRect::UiRect() {
}

UiRect::~UiRect() {
}

void UiRect::move(Vector2 distance) {
    bounds.x += distance.x;
    bounds.y += distance.y;
}

void UiRect::setPos(Vector2 pos) {
    Vector2 diff;
    diff.x = pos.x - this->bounds.x;
    diff.y = pos.y - this->bounds.y;
    move(diff);
}

void UiRect::draw() {
  DrawRectangleRec(bounds, bgColor);
}

void UiRect::receiveMousePos(Vector2 mousePos) {
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

void UiRect::receiveMouseDown(Vector2 mousePos) {
}

void UiRect::receiveMouseUp(Vector2 mousePos) {
  if (contains(mousePos)) {
    if (onClick) {
      onClick(this);
    }
  }
}

void UiRect::setColor(Color c) {
  bgColor = c;
}

void UiRect::setSize(Vector2 size) {
  bounds.width = size.x;
  bounds.height = size.y;
}

bool UiRect::contains(Vector2 mousePos) {
  return CheckCollisionPointRec(mousePos, bounds);
}

UiDropDown::UiDropDown(std::string label, std::vector<std::string> options) {
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
    std::shared_ptr<UiText> uiOpt(new UiText(opt));
    uiOpt->setPos({static_cast<float>(padding), (static_cast<float>(padding) * 2 + biggestSize.y) * (i+1) + padding});
    uiOpt->setColor({255, 255, 255, 255});

    std::shared_ptr<UiRect> optBg(new UiRect());
    optBg->setPos({0, (static_cast<float>(padding) * 2 + biggestSize.y) * (i+1)});
    optBg->setSize({ biggestSize.x + padding * 2, biggestSize.y + padding * 2 });
    optBg->setColor({30, 30, 30, 255});
    optBg->setOnMouseEnter([](Ui* p) {
        UiRect* bg = static_cast<UiRect*>(p);
        bg->setColor({155, 155, 155, 255});
    });
    optBg->setOnMouseExit([](Ui* p) {
        UiRect* bg = static_cast<UiRect*>(p);
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
      UiText* lbl = static_cast<UiText*>(l);
      lbl->setColor({0, 255, 0, 255});
  });
  this->label.setOnMouseExit([](Ui* l) {
      UiText* lbl = static_cast<UiText*>(l);
      lbl->setColor({255, 255, 255, 255});
  });

  labelBg.setColor({255, 255, 255, 30});
  labelBg.setSize(btnSize);

  pos = {0, 0};
}

UiDropDown::~UiDropDown() {
}

void UiDropDown::move(Vector2 distance) {
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

void UiDropDown::setPos(Vector2 pos) {
    Vector2 diff;
    diff.x = pos.x - this->pos.x;
    diff.y = pos.y - this->pos.y;
    move(diff);
}

void UiDropDown::draw() {
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

void UiDropDown::receiveMousePos(Vector2 mousePos) {
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

void UiDropDown::receiveMouseDown(Vector2 mousePos) {
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

void UiDropDown::receiveMouseUp(Vector2 mousePos) {
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

void UiDropDown::setOnSelected(std::function<void(std::string)> f) {
  onSelected = f;
}

void UiDropDown::select(size_t i) {
  if (onSelected) {
    onSelected(options[i]);
  }
}

Ui3DViewport::Ui3DViewport() {
  camera.position = { 20.0f, 20.0f, 20.0f };
  camera.target = { 0.0f, 0.0f, 0.0f };
  camera.up = { 0.0f, 1.0f, 0.0f };
  camera.fovy = 45.0f;
  camera.projection = CAMERA_PERSPECTIVE;
}

Ui3DViewport::~Ui3DViewport() {
}

void Ui3DViewport::move(Vector2 distance) {
}

void Ui3DViewport::setPos(Vector2 pos) {
}

void Ui3DViewport::draw() {
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

void Ui3DViewport::receiveMousePos(Vector2 mousePos) {
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

void Ui3DViewport::receiveMouseDown(Vector2 mousePos) {
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

void Ui3DViewport::receiveMouseUp(Vector2 mousePos) {
}

void Ui3DViewport::setScene(std::shared_ptr<Scene> scene) {
  this->scene = scene;
}

void Ui3DViewport::setAreaPointers(
  Rectangle* screenRect,
  Vector2* screenPos,
  RenderTexture* texture
) {
  areaSreenRect = screenRect;
  areaScreenPos = screenPos;
  areaTexture = texture;
}

Ray Ui3DViewport::getNonOffsetMouseRay(Vector2 mousePos) {
  Vector2 offset = { 0.0, 0.0 };
  if (areaSreenRect != nullptr && areaScreenPos != nullptr && areaTexture != nullptr) {
    offset.x = (areaSreenRect->width - areaTexture->texture.width) / 2.0;
    offset.y = (areaSreenRect->height - areaTexture->texture.height) / 2.0;
  }

  Ray ray = { 0 };
  ray = GetMouseRay(Vector2Subtract(mousePos, offset), camera);

  return ray;
}

UiToolList::UiToolList() {
  btnNames.push_back("Sketch");
  btnNames.push_back("Point");
  btnNames.push_back("Line");
  pos = { 0, 0 };

  btnSize = {80, 80};
  margin = 4;

  int i = 0;
  for (std::string& btnName : btnNames) {
    std::shared_ptr<UiText> btnLbl(new UiText(btnName));
    Vector2 textSize = MeasureTextEx(colorscheme->font, btnName.c_str(), 20, 1);
    btnLbl->setPos({(btnSize.x + margin) * i + (btnSize.x - textSize.x) / 2.0f, (btnSize.y - textSize.y) / 2.0f});
    btnLbl->setColor({255, 255, 255, 255});
    btnLbls.push_back(btnLbl);

    std::shared_ptr<UiRect> btnBg(new UiRect());
    btnBg->setSize(btnSize);
    btnBg->setColor(colorscheme->secondary);
    btnBg->setPos({(btnSize.x + margin) * i, 0});
    btnBg->setOnMouseEnter([](Ui* p) {
        UiRect* bg = static_cast<UiRect*>(p);
        bg->setColor(colorscheme->secondaryVariant);
    });
    btnBg->setOnMouseExit([](Ui* p) {
        UiRect* bg = static_cast<UiRect*>(p);
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
}

UiToolList::~UiToolList() {
}

void UiToolList::move(Vector2 distance) {
  for (auto& lbl : btnLbls) {
    lbl->move(distance);
  }
  for (auto& bg : btnBgs) {
    bg->move(distance);
  }
}

void UiToolList::setPos(Vector2 pos) {
    Vector2 diff;
    diff.x = pos.x - this->pos.x;
    diff.y = pos.y - this->pos.y;
    move(diff);
}

void UiToolList::draw() {
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
      };
      if (ApplicationState::getInstance()->sketchModeActive) {
        lbl->draw();
      }
    }
    ++i;
  }
}

void UiToolList::receiveMousePos(Vector2 mousePos) {
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

void UiToolList::receiveMouseDown(Vector2 mousePos) {
  for (auto& bg : btnBgs) {
    bg->receiveMouseDown(mousePos);
  }
  for (auto& lbl : btnLbls) {
    lbl->receiveMouseDown(mousePos);
  }
}

void UiToolList::receiveMouseUp(Vector2 mousePos) {
  for (auto& bg : btnBgs) {
    bg->receiveMouseUp(mousePos);
  }
  for (auto& lbl : btnLbls) {
    lbl->receiveMouseUp(mousePos);
  }
}

