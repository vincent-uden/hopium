#include "TextInput.h"
#include "raylib.h"
#include <iostream>

namespace Ui {

TextInput::TextInput() {
  pos.x = 0;
  pos.y = 0;
  setText("");
  color = { 0, 0, 0, 255 };
}

TextInput::TextInput(std::string text) {
  pos.x = 0;
  pos.y = 0;
  setText(text);
  color = { 0, 0, 0, 255 };
}

TextInput::~TextInput() {
}

void TextInput::move(Vector2 distance) {
  pos.x += distance.x;
  pos.y += distance.y;
}

void TextInput::setPos(Vector2 pos) {
  Vector2 diff;
  diff.x = pos.x - this->pos.x;
  diff.y = pos.y - this->pos.y;
  move(diff);
  calculateCursorLocation();
}

void TextInput::draw() {
  calculateCursorLocation();
  DrawTextEx(colorscheme->font, text.c_str(), baseDrawingPos(), 20, 1, color);
  DrawLineEx(Vector2Add(pos, cursorStart), Vector2Add(pos, cursorEnd), 1.0, color);
}

void TextInput::receiveMousePos(Vector2 mousePos) {
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

void TextInput::receiveMouseDown(Vector2 mousePos) {
}

void TextInput::receiveMouseUp(Vector2 mousePos) {
  if ( mousePos.x > pos.x && mousePos.x < pos.x + size.x ) {
    if ( mousePos.y > pos.y && mousePos.y < pos.y + size.y ) {
      if (onClick) {
        onClick(this);
      }
    }
  }
}

void TextInput::receiveMouseWheel(Vector2 mousePos, float movement) {
}

Vector2 TextInput::getSize() {
  return size;
}

void TextInput::setAlignment(TextAlignment align) {
  this->align = align;
  calculateCursorLocation();
}

void TextInput::setColor(Color c) {
  color = c;
}

void TextInput::setCursor(int cursor) {
  this->cursor = cursor;
  calculateCursorLocation();
}

void TextInput::setText(std::string text) {
  this->text = text;
  size = MeasureTextEx(GetFontDefault(), text.c_str(), 20, 1);
  calculateCursorLocation();
}

Vector2 TextInput::baseDrawingPos() {
  Vector2 drawPos = pos;
  switch (align) {
  case LEFT:
    break;
  case CENTER:
    drawPos.x -= size.x / 2.f;
    break;
  case RIGHT:
    drawPos.x -= size.x;
    break;
  }
  return drawPos;
}

void TextInput::calculateCursorLocation() {
  std::string before = text.substr(0, cursor);
  Vector2 beforeSize = MeasureTextEx(colorscheme->font, before.c_str(), 20, 1);
  cursorStart.x = beforeSize.x + 1;
  cursorEnd.x = beforeSize.x + 1;
  cursorEnd.y = 20;
}

}
