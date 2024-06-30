#include "Row.h"

namespace Ui {

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

void Row::receiveMouseWheel(Vector2 mousePos, float movement) {
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

}
