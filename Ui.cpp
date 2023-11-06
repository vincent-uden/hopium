#include "Ui.h"
#include "raylib.h"
#include <cmath>

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
    DrawTextEx(GetFontDefault(), text.c_str(), pos, 20, 1, color);
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

UiDropDown::UiDropDown(std::string label, std::vector<std::string> options) {
  this->options = options;
  btnSize = MeasureTextEx(GetFontDefault(), label.c_str(), 20, 1);

  size_t widest = 0;
  Vector2 currSize;
  Vector2 biggestSize = MeasureTextEx(GetFontDefault(), options[0].c_str(), 20, 1);
  for (size_t i = 0; i < options.size(); ++i) {
    currSize = MeasureTextEx(GetFontDefault(), options[i].c_str(), 20, 1);
    if (currSize.y > biggestSize.y) {
      biggestSize = currSize;
      widest = i;
    }
  }

  listSize = { biggestSize.y, 20.0f * options.size() };

  this->label.setText(label);
  this->label.setColor({255, 255, 255, 255});

  this->label.setOnMouseEnter([](Ui* l) {
      UiText* lbl = static_cast<UiText*>(l);
      lbl->setColor({0, 255, 0, 255});
  });
  this->label.setOnMouseExit([](Ui* l) {
      UiText* lbl = static_cast<UiText*>(l);
      lbl->setColor({255, 255, 255, 255});
  });
}

UiDropDown::~UiDropDown() {
}

void UiDropDown::move(Vector2 distance) {
  label.move(distance);
}

void UiDropDown::setPos(Vector2 pos) {
    Vector2 diff;
    diff.x = pos.x - this->pos.x;
    diff.y = pos.y - this->pos.y;
    move(diff);
}

void UiDropDown::draw() {
  label.draw();
}

void UiDropDown::receiveMousePos(Vector2 mousePos) {
  label.receiveMousePos(mousePos);

  // TODO: If open, check big collision, if closed check small
  /*
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
  */
}

void UiDropDown::setOnSelected(std::function<void(std::string)>) {
}

