#include "Rect.h"

namespace Ui {

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

void Rect::receiveMouseWheel(Vector2 mousePos, float movement) {
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

}
