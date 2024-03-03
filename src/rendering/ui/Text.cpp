#include "Text.h"

namespace Ui {

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
  Vector2 drawPos = pos;
  switch (align) {
  case LEFT:
    break;
  case CENTER:
    pos.x -= size.x / 2.f;
    break;
  case RIGHT:
    pos.x -= size.x;
    break;
  }
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

void Text::receiveMouseWheel(Vector2 mousePos, float movement) {
}

Vector2 Text::getSize() {
  return size;
}

void Text::setAlignment(TextAlignment align) {
  this->align = align;
}

void Text::setColor(Color c) {
  color = c;
}

void Text::setText(std::string text) {
  this->text = text;
  size = MeasureTextEx(GetFontDefault(), text.c_str(), 20, 1);
}

}
