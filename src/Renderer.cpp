#include "Renderer.h"
#include "raylib.h"
#include <cmath>
#include <limits>

Boundary::Boundary(std::shared_ptr<Colorscheme> colorscheme) {
  this->colorscheme = colorscheme;
}

Boundary::~Boundary() {
}

bool Boundary::canCollapse() {
  return side1.size() == 1 && side2.size() == 1;
}

void Boundary::collapse(std::shared_ptr<Area> toDelete) {
  if (canCollapse()) {
    toDelete->active = false;

    if (side1.empty()) {
      side2[0]->deleteBoundary(this);
    } else {
      side1[0]->deleteBoundary(this);
    }
    active = false;
  }
}

void Boundary::deleteArea(Area* toDelete) {
  int j = -1;
  for (int i = 0; i < side1.size(); ++i) {
    if (side1[i].get() == toDelete) {
      j = i;
      break;
    }
  }

  if (j != -1) {
    auto it = side1.begin();
    std::advance(it, j);
    side1.erase(it);
    return;
  }

  for (int i = 0; i < side2.size(); ++i) {
    if (side2[i].get() == toDelete) {
      j = i;
      break;
    }
  }

  if (j != -1) {
    auto it = side2.begin();
    std::advance(it, j);
    side2.erase(it);
  }
}

void Boundary::draw() {
  Vector2 startPos = side2[0]->screenPos;
  Vector2 endPos = side2[0]->screenPos;
  switch (orientation) {
  case VERTICAL:
    endPos.y += extent();
    break;
  case HORIZONTAL:
    endPos.x += extent();
    break;
  }
  DrawLineEx(startPos, endPos, thickness, colorscheme->borderColor);
}

void Boundary::moveBoundary(Vector2 screenPos) {
  float diff = 0;
  switch (orientation) {
  case VERTICAL:
    diff = screenPos.x - side2[0]->screenPos.x;
    // --- Ensure minimum extent of areas
    for (auto& area : side1) {
      if (area->screenRect.width + diff < area->minimumExtent) {
        diff = area->minimumExtent - area->screenRect.width;
      }
    }
    for (auto& area : side2) {
      if (area->screenRect.width - diff < area->minimumExtent) {
        diff = -(area->minimumExtent - area->screenRect.width);
      }
    }
    // ---
    for (auto& area : side1) {
      area->screenRect.width += diff;
    }
    for (auto& area : side2) {
      area->screenPos.x += diff;
      area->screenRect.width -= diff;
    }
    break;
  case HORIZONTAL:
    diff = screenPos.y - side2[0]->screenPos.y;
    // --- Ensure minimum extent of areas
    for (auto& area : side1) {
      if (area->screenRect.height + diff < area->minimumExtent) {
        diff = area->minimumExtent - area->screenRect.height;
      }
    }
    for (auto& area : side2) {
      if (area->screenRect.height - diff < area->minimumExtent) {
        diff = -(area->minimumExtent - area->screenRect.height);
      }
    }
    // ---
    for (auto& area : side1) {
      area->screenRect.height += diff;
    }
    for (auto& area : side2) {
      area->screenPos.y += diff;
      area->screenRect.height -= diff;
    }
    break;
  }


}

float Boundary::extent() {
  float total = 0;

  switch(orientation) {
  case VERTICAL:
    for (auto& area : side2) {
      total += area->screenRect.height;
    }
    break;
  case HORIZONTAL:
    for (auto& area : side2) {
      total += area->screenRect.width;
    }
    break;
  }

  return total;
}

float Boundary::distanceToPoint(Vector2 pos) {
  switch (orientation) {
  case VERTICAL:
    if (pos.y > side2[0]->screenPos.y && pos.y < side2[0]->screenPos.y + extent()) {
      return std::fabs(side2[0]->screenPos.x - pos.x);
    }
  case HORIZONTAL:
    if (pos.x > side2[0]->screenPos.x && pos.x < side2[0]->screenPos.x + extent()) {
      return std::fabs(side2[0]->screenPos.y - pos.y);
    }
  }

  return std::numeric_limits<float>::infinity();
}

Area::Area(int screenW, int screenH, Rectangle screenRect, Vector2 screenPos, int id, std::shared_ptr<Colorscheme> colorscheme) {
  paneTexture = LoadRenderTexture(screenW, screenH);
  this->screenRect = screenRect;
  this->screenPos = screenPos;
  this->colorscheme = colorscheme;

  paneId = id;
}

Area::~Area() {
  UnloadRenderTexture(paneTexture);
}

void Area::draw() {
  BeginTextureMode(paneTexture);
  ClearBackground(colorscheme->background);
  for (std::shared_ptr<Ui>& ui : contents) {
    ui->draw();
  }
  EndTextureMode();
  Rectangle drawRect = screenRect;
  drawRect.y = -drawRect.height;
  drawRect.height *= -1.f;
  switch (anchor) {
  case LEFT:
    break;
  case CENTER:
    drawRect.x = (paneTexture.texture.width - screenRect.width) / 2.0f;
    drawRect.y = (paneTexture.texture.height - screenRect.height) / 2.0f;
    break;
  case RIGHT:
    drawRect.x = paneTexture.texture.width - screenRect.width;
    break;
  }
  drawRect.x = std::round(drawRect.x);
  drawRect.y = std::round(drawRect.y);
  drawRect.width = std::round(drawRect.width);
  drawRect.height = std::round(drawRect.height);
  DrawTextureRec(paneTexture.texture, drawRect, screenPos, WHITE);
}

bool Area::containsPoint(Vector2 localPos) {
  if (localPos.x > screenPos.x && localPos.x < screenPos.x + screenRect.width) {
    if (localPos.y > screenPos.y && localPos.y < screenPos.y + screenRect.height) {
      return true;
    }
  }

  return false;
}

void Area::receiveMousePos(Vector2 mousePos) {
  if (containsPoint(mousePos)) {
    if (!hovered) {
      if (onMouseEnter != nullptr) {
        onMouseEnter(this);
      }
    }
    hovered = true;
    for (auto& ui : contents) {
      ui->receiveMousePos(Vector2Subtract(mousePos, screenPos));
    }
  } else {
    if (hovered) {
      if (onMouseExit != nullptr) {
        onMouseExit(this);
      }
    }
    hovered = false;
    // If the mouse isn't in the active area, treat it as being outside the
    // entire window.
    for (auto& ui : contents) {
      ui->receiveMousePos({-100.f, -100.f});
    }
  }
}

void Area::addUi(std::shared_ptr<Ui>& ui) {
  contents.push_back(std::move(ui));
}

void Area::dumpInfo() {
  std::cout << "--------------" << std::endl;
  std::cout << "Pane " << paneId << std::endl;
  std::cout << "Pos: " << screenPos.x << " " << screenPos.y << std::endl;
  std::cout << "Rect: " << screenRect.x << " " << screenRect.y << " " << screenRect.width << " " << screenRect.height << std::endl;
}

void Area::deleteBoundary(Boundary* toDelete) {
  if (leftBdry.get() == toDelete) {
    leftBdry.reset();
  } else if (rightBdry.get() == toDelete) {
    rightBdry.reset();
  } else if (upBdry.get() == toDelete) {
    upBdry.reset();
  } else if (downBdry.get() == toDelete) {
    downBdry.reset();
  }
}

void Area::deleteThisFromBoundaries() {
  if (leftBdry) {
    leftBdry->deleteArea(this);
  }
}

Renderer::Renderer(int screenW, int screenH) {
  this->screenW = screenW;
  this->screenH = screenH;

  font = LoadFontEx("../assets/Geist/Geist-Regular.otf", 20, 0, 250);
  colorscheme = std::shared_ptr<Colorscheme>(new Colorscheme(&font));

  Ui::colorscheme = colorscheme;

  areas.push_back(std::shared_ptr<Area>(new Area(
    screenW,
    screenH,
    { 0, 0, static_cast<float>(screenW), static_cast<float>(screenH) },
    { 0, 0 },
    nextPaneId++,
    colorscheme
  )));
}

Renderer::~Renderer() {
  areas.clear();

  UnloadFont(font);
}

void Renderer::draw() {
  BeginDrawing();
  ClearBackground(BLACK);

  for (auto area : areas) {
    area->draw();
  }

  for (auto bdry : boundaries) {
    bdry->draw();
  }

  DrawFPS(screenW - 100, 10);
  EndDrawing();
}

void Renderer::receiveMousePos(Vector2 mousePos) {
  for (auto& area : areas) {
    area->receiveMousePos(mousePos);
  }

  if (grabbed == nullptr) {
    std::shared_ptr<Boundary> hovered = findBoundary(mousePos, mouseBoundaryTolerance);
    for (auto& bdry : boundaries) {
      bdry->thickness = 3;
    }
    if (hovered) {
        hovered->thickness = 6;
    }
  } else {
    // Move grabbed boundary
    grabbed->moveBoundary(mousePos);
    grabbed->thickness = 6;
  }
}

void Renderer::mouseDown(Vector2 mousePos) {
  // If close to a border, grab it
  if (!grabbed) {
    std::shared_ptr<Boundary> hovered = findBoundary(mousePos, mouseBoundaryTolerance);
    if (hovered) {
      grabbed = hovered;
    }
  }
}

void Renderer::mouseUp(Vector2 mousePos) {
  // If a border is grabbed, release it
  if (grabbed) {
    grabbed = nullptr;
  }
}

// Split a parent pane horizontally and give it one child which occupies the
// right half of the area intially owned by the parent.
void Renderer::splitPaneHorizontal(Vector2 mousePos) {
  std::shared_ptr<Area> toSplit = findPane(mousePos);

  std::shared_ptr<Area> newArea = std::shared_ptr<Area>(new Area(
    screenW,
    screenH,
    { 0, 0, toSplit->screenRect.width / 2.0f, toSplit->screenRect.height },
    { toSplit->screenPos.x + toSplit->screenRect.width / 2.0f, toSplit->screenPos.y },
    nextPaneId++,
    colorscheme
  ));
  toSplit->screenRect.width /= 2.0f;

  std::shared_ptr<Boundary> bdry = std::shared_ptr<Boundary>(new Boundary(colorscheme));
  bdry->side1.push_back(toSplit);
  bdry->side2.push_back(newArea);
  newArea->leftBdry = bdry;

  // There's already an area below
  if (toSplit->rightBdry) {
    toSplit->rightBdry->deleteArea(toSplit.get());
    toSplit->rightBdry->side1.push_back(newArea);

    newArea->rightBdry = toSplit->rightBdry;
  }
  if (toSplit->downBdry) {
    toSplit->downBdry->side1.push_back(newArea);
    newArea->downBdry = toSplit->downBdry;
  }
  if (toSplit->upBdry) {
    toSplit->upBdry->side2.push_back(newArea);
    newArea->upBdry = toSplit->upBdry;
  }

  toSplit->rightBdry = bdry;

  bdry->orientation = BoundaryOrientation::VERTICAL;
  bdry->id = nextBdryId++;
  boundaries.push_back(bdry);
  areas.push_back(newArea);
}

// Split a parent pane vertically and give it one child which occupies the
// bottom half of the area intially owned by the parent.
void Renderer::splitPaneVertical(Vector2 mousePos) {
  std::shared_ptr<Area> toSplit = findPane(mousePos);

  std::shared_ptr<Area> newArea = std::shared_ptr<Area>(new Area(
    screenW,
    screenH,
    { 0, 0, toSplit->screenRect.width, toSplit->screenRect.height / 2.0f },
    { toSplit->screenPos.x, toSplit->screenPos.y + toSplit->screenRect.height / 2.0f },
    nextPaneId++,
    colorscheme
  ));
  toSplit->screenRect.height /= 2.0f;

  std::shared_ptr<Boundary> bdry = std::shared_ptr<Boundary>(new Boundary(colorscheme));
  bdry->side1.push_back(toSplit);
  bdry->side2.push_back(newArea);
  newArea->upBdry = bdry;

  // There's already an area below
  if (toSplit->downBdry) {
    toSplit->downBdry->deleteArea(toSplit.get());
    toSplit->downBdry->side1.push_back(newArea);

    newArea->downBdry = toSplit->downBdry;
  }
  if (toSplit->leftBdry) {
    toSplit->leftBdry->side2.push_back(newArea);
    newArea->leftBdry = toSplit->leftBdry;
  }
  if (toSplit->rightBdry) {
    toSplit->rightBdry->side1.push_back(newArea);
    newArea->rightBdry = toSplit->rightBdry;
  }

  toSplit->downBdry = bdry;

  bdry->orientation = BoundaryOrientation::HORIZONTAL;
  bdry->id = nextBdryId++;
  boundaries.push_back(bdry);
  areas.push_back(newArea);
}

// Recursively dump information about all panes for debugging purposes.
void Renderer::dumpPanes() {
  std::cout << std::endl << "Pane information" << std::endl;
  for (auto& area : areas) {
    area->dumpInfo();
  }
}

// Finds the pane which contains the position `pos`. Since the root pane covers
// the entire screen, this will **never** return a null pointer.
std::shared_ptr<Area> Renderer::findPane(Vector2 pos) {
  for (auto area : areas) {
    if (area->containsPoint(pos)) {
      return area;
    }
  }

  return nullptr;
}

std::shared_ptr<Boundary> Renderer::findBoundary(Vector2 pos, float radius) {
  size_t closest = -1;
  float closestDist = std::numeric_limits<float>::infinity();
  size_t i = 0;
  for (auto& bdry : boundaries) {
    float dist = bdry->distanceToPoint(pos);
    if (dist < closestDist) {
      closestDist = dist;
      closest = i;
    }
    bdry->thickness = 3;
    ++i;
  }

  if (closestDist < radius)  {
    return boundaries[closest];
  } else {
    return nullptr;
  }
}