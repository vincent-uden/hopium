#include "Icon.h"

namespace Ui {

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

void Icon::receiveMouseWheel(Vector2 mousePos, float movement) {
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
  this->tooltip = tooltip;
}

void Icon::setBgColor(Color color) {
  bg->setColor(color);
}

}
