#include "STreeViewer.h"

namespace Ui {

STreeViewer::STreeViewer() {
  pos.x = 0;
  pos.y = 0;
}

STreeViewer::~STreeViewer() {
}

void STreeViewer::move(Vector2 distance) {
  pos.x += distance.x;
  pos.y += distance.y;

  for (const std::shared_ptr<GraphViewer>& g: nodes) {
    if (g) {
      g->move(distance);
    }
  }
}

void STreeViewer::setPos(Vector2 pos) {
    Vector2 diff;
    diff.x = pos.x - this->pos.x;
    diff.y = pos.y - this->pos.y;
    move(diff);
}

void STreeViewer::draw() {
  setZoom();
  for (const std::shared_ptr<GraphViewer>& g: nodes) {
    DrawRectangleRec(g->boundingBox(10), {205, 205, 255, 30});
    g->draw();
  }
}

void STreeViewer::receiveMousePos(Vector2 mousePos) {
  if (ApplicationState::getInstance()->holdingRotate) {
    Vector2 diff = Vector2Subtract(mousePos, lastMousePos);
    move(diff);
  }

  for (const std::shared_ptr<GraphViewer>& g: nodes) {
    g->receiveMousePos(mousePos);
  }

  lastMousePos = mousePos;
}

void STreeViewer::receiveMouseDown(Vector2 mousePos) {
  for (const std::shared_ptr<GraphViewer>& g: nodes) {
    g->receiveMouseDown(mousePos);
  }
}

void STreeViewer::receiveMouseWheel(Vector2 mousePos, float movement) {
}

void STreeViewer::receiveMouseUp(Vector2 mousePos) {
  for (const std::shared_ptr<GraphViewer>& g: nodes) {
    g->receiveMouseUp(mousePos);
  }
}

Vector2 STreeViewer::getSize() {
  return { areaScreenRect->width, areaScreenRect->height };
}

void STreeViewer::setAreaPointers(
  Rectangle* screenRect,
  Vector2* screenPos,
  RenderTexture* texture
) {
  areaScreenRect = screenRect;
  areaScreenPos = screenPos;
  areaTexture = texture;
  for (const std::shared_ptr<GraphViewer>& g: nodes) {
    g->setAreaPointers(screenRect, screenPos, texture);
  }
}

void STreeViewer::setSTree(std::shared_ptr<STree> stree) {
  this->stree = stree;
  traverse(this->stree, {0, 0});
}

void STreeViewer::traverse(std::shared_ptr<STree> stree, Vector2 offset) {
  std::shared_ptr<GraphViewer> graphViewer = std::make_shared<GraphViewer>();
  graphViewer->setAreaPointers(areaScreenRect, areaScreenPos, areaTexture);
  graphViewer->setGraph(stree->node);
  graphViewer->setPos(offset);
  float zoom = ApplicationState::getInstance()->zoom;
  if (stree->left) {
    traverse(
      stree->left,
      Vector2Add(
        offset,
        { -std::pow(2.0f, static_cast<float>(stree->depth() - 1)) * xOffset * zoom, yOffset * zoom }
      )
    );
  }
  if (stree->right) {
    traverse(
      stree->right,
      Vector2Add(
        offset,
        { std::pow(2.0f, static_cast<float>(stree->depth() - 1)) * xOffset * zoom, yOffset * zoom }
      )
    );
  }
  nodes.push_back(graphViewer);
}

void STreeViewer::setZoom() {
  for (const std::shared_ptr<GraphViewer>& g: nodes) {
    g->setPos(Vector2Scale(g->getPos(), ApplicationState::getInstance()->zoom / lastZoom));
  }
  lastZoom = ApplicationState::getInstance()->zoom;
}

}
