#include "Ui.h"
#include "raylib.h"
#include <cmath>
#include <iostream>

// TODO: Make this file use Colorscheme from Renderer.h

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
        onMouseEnter(this);
      }
      hovered = true;
      return;
    }
  }

  if (hovered) {
    onMouseExit(this);
  }

  hovered = false;
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
      onMouseEnter(this);
    }
    hovered = true;
  } else {
    if (hovered) {
      onMouseExit(this);
    }
    hovered = false;
  }
}

void UiRect::setColor(Color c) {
  bgColor = c;
}

void UiRect::setSize(Vector2 size) {
  bounds.width = size.x;
  bounds.height = size.y;
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

void UiDropDown::setOnSelected(std::function<void(std::string)>) {
}


Ui3DViewport::Ui3DViewport() {
  camera.position = { 50.0f, 50.0f, 50.0f };
  camera.target = { 0.0f, 10.0f, 0.0f };
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

  DrawGrid(20, 10.0f);

  if (scene) {
    for (size_t i = 0; i < scene->nBodies(); ++i) {
      std::shared_ptr<RasterBody> body = scene->getBody(i);
      DrawModel(body->model, body->pos, 5.f, WHITE);
    }
  }

  EndMode3D();
}

void Ui3DViewport::receiveMousePos(Vector2 mousePos) {
}

void Ui3DViewport::setScene(std::shared_ptr<Scene> scene) {
  this->scene = scene;
}
