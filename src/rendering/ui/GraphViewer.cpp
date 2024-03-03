#include "GraphViewer.h"

namespace Ui {

GraphViewer::GraphViewer() {
  pos.x = 0;
  pos.y = 0;
}

GraphViewer::~GraphViewer() {
}

void GraphViewer::move(Vector2 distance) {
  pos.x += distance.x;
  pos.y += distance.y;
}

void GraphViewer::setPos(Vector2 pos) {
    Vector2 diff;
    diff.x = pos.x - this->pos.x;
    diff.y = pos.y - this->pos.y;
    move(diff);
}

void GraphViewer::draw() {
  // Update positions, forces and velocities (Using verlet integration)
  for (size_t i = 0; i < graph->vertices.size(); ++i) {
    nodePos[i].x += nodeVel[i].x * dt + nodeAcc[i].x * dt * dt * 0.5;
    nodePos[i].y += nodeVel[i].y * dt + nodeAcc[i].y * dt * dt * 0.5;

    Vector2 a = Vector2Scale(nodeAcc[i], 0.5 * dt);
    nodeVel[i].x += a.x;
    nodeVel[i].y += a.y;

    nodeAcc[i].x = 0;
    nodeAcc[i].y = 0;
  }
  for (size_t i = 0; i < graph->vertices.size(); ++i) {
    Vector2 direction = Vector2Normalize(nodePos[i]);
    float mag = std::max(Vector2LengthSqr(nodePos[i]), 0.1f);
    nodeAcc[i].x -= direction.x * mag * centralAttraction;
    nodeAcc[i].y -= direction.y * mag * centralAttraction;

    for (size_t j = i + 1; j < graph->vertices.size(); ++j) {
      Vector2 diff = Vector2Subtract(nodePos[i], nodePos[j]);
      Vector2 f = Vector2Scale(Vector2Normalize(diff), 1.0f / Vector2LengthSqr(diff));
      nodeAcc[i].x += f.x * pushForce;
      nodeAcc[i].y += f.y * pushForce;
      nodeAcc[j].x -= f.x * pushForce;
      nodeAcc[j].y -= f.y * pushForce;

      if (graph->vertices[i]->isConnected(graph->vertices[j])) {
        float springL = Vector2Length(diff);
        nodeAcc[i].x -= diff.x * std::pow(springLength - springL, 2) * pullForce;
        nodeAcc[i].y -= diff.y * std::pow(springLength - springL, 2) * pullForce;
        nodeAcc[j].x += diff.x * std::pow(springLength - springL, 2) * pullForce;
        nodeAcc[j].y += diff.y * std::pow(springLength - springL, 2) * pullForce;
      }
    }
    Vector2 a = Vector2Scale(nodeAcc[i], 0.5 * dt);
    nodeVel[i].x += a.x;
    nodeVel[i].y += a.y;
    nodeVel[i].x *= velocityDamping;
    nodeVel[i].y *= velocityDamping;
  }
  if (grabbedId != -1) {
    Vector2 mousePosInWorldSpace = toGraphSpace(lastMousePos);
    nodePos[grabbedId].x = mousePosInWorldSpace.x;
    nodePos[grabbedId].y = mousePosInWorldSpace.y;
    nodeVel[grabbedId].x = 0;
    nodeVel[grabbedId].y = 0;
    nodeAcc[grabbedId].x = 0;
    nodeAcc[grabbedId].y = 0;
  }

  // Drawing
  size_t i = 0;
  for (const std::shared_ptr<GeometricElement>& v: graph->vertices) {
    Vector2 renderPos = toScreenSpace(nodePos[i]);
    DrawTextEx(
      colorscheme->font,
      v->label.c_str(),
      Vector2Add(renderPos, { -5, -30 }),
      20,
      1,
      colorscheme->onBackground
    );
    ++i;

    size_t j = 0;
    for (const std::shared_ptr<GeometricElement>& u: graph->vertices) {
      if (v->isVirtuallyConnected(u)) {
        DrawLineEx(
          renderPos,
          toScreenSpace(nodePos[j]),
          edgeThickness,
          {255, 0, 0, 100}
        );
      } else if (v->isConnected(u)) {
        DrawLineEx(
          renderPos,
          toScreenSpace(nodePos[j]),
          edgeThickness,
          {255, 255, 255, 100}
        );
      }
      ++j;
    }
  }

  i = 0;
  for (const std::shared_ptr<GeometricElement>& v: graph->vertices) {
    DrawCircleV(
      toScreenSpace(nodePos[i]),
      10,
      i == grabbedId ? Color {0, 255, 0, 255} : (i == hoveredId ? Color {0, 255, 0, 150} : Color {255, 255, 0, 150})
    );
    ++i;
  }
}

void GraphViewer::receiveMousePos(Vector2 mousePos) {
  lastMousePos = mousePos;

  hoveredId = -1;
  for (size_t i = 0; i < nodePos.size(); ++i) {
    if (Vector2Distance(toScreenSpace(nodePos[i]), mousePos) < selectThreshold) {
      hoveredId = i;
      break;
    }
  }
}

void GraphViewer::receiveMouseDown(Vector2 mousePos) {
  if (hoveredId != -1) {
    grabbedId = hoveredId;
  }
}

void GraphViewer::receiveMouseUp(Vector2 mousePos) {
  grabbedId = -1;
}

void GraphViewer::receiveMouseWheel(Vector2 mousePos, float movement) {
}

Vector2 GraphViewer::getSize() {
  return { areaScreenRect->width, areaScreenRect->height };
}

void GraphViewer::setAreaPointers(
  Rectangle* screenRect,
  Vector2* screenPos,
  RenderTexture* texture
) {
  areaScreenRect = screenRect;
  areaScreenPos = screenPos;
  areaTexture = texture;
}

void GraphViewer::setGraph(std::shared_ptr<ConstraintGraph> graph) {
  this->graph = graph;
  nodePos.clear();
  for (const std::shared_ptr<GeometricElement>& e: this->graph->vertices) {
    nodePos.push_back({ (std::rand() % 1000) / 1000.0f - 0.5f, (std::rand() % 1000) / 1000.0f - 0.5f });
    nodeVel.push_back({ 0.0, 0.0 });
    nodeAcc.push_back({ 0.0, 0.0 });
  }
}

Vector2 GraphViewer::getPos() {
  return pos;
}

Rectangle GraphViewer::boundingBox(float padding) {
  Vector2 min = { std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
  Vector2 max = { -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max() };
  for (const Vector2& p: nodePos) {
    if (p.x < min.x) { min.x = p.x; }
    if (p.y < min.y) { min.y = p.y; }
    if (p.x > max.x) { max.x = p.x; }
    if (p.y > max.y) { max.y = p.y; }
  }
  min = toScreenSpace(min);
  max = toScreenSpace(max);
  min.x -= padding;
  min.y -= padding;
  max.x += padding;
  max.y += padding;

  return Rectangle { min.x, min.y, max.x - min.x, max.y - min.y };
}

Vector2 GraphViewer::toScreenSpace(const Vector2 p) {
  Vector2 out = p;
  out = Vector2Scale(out, scale);
  out = Vector2Add(out, Vector2Scale(Vector2(areaScreenRect->width, areaScreenRect->height), 0.5f));
  out = Vector2Add(pos, out);
  return out;
}

Vector2 GraphViewer::toGraphSpace(const Vector2 p) {
  Vector2 out = p;
  out = Vector2Subtract(out, Vector2Scale(Vector2(areaScreenRect->width, areaScreenRect->height), 0.5f));
  out = Vector2Subtract(out, pos);
  out = Vector2Scale(out, 1/scale);
  return out;
}

}
