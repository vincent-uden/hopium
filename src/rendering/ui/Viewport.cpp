#include "Viewport.h"

namespace Ui {

Viewport::Viewport() {
  camera.position = { 20.0f, 20.0f, 20.0f };
  camera.target = { 0.0f, 0.0f, 0.0f };
  camera.up = { 0.0f, 1.0f, 0.0f };
  camera.fovy = 45.0f;
  camera.projection = CAMERA_PERSPECTIVE;
}

Viewport::~Viewport() {
}

void Viewport::move(Vector2 distance) {
}

void Viewport::setPos(Vector2 pos) {
}

void Viewport::draw() {
  BeginMode3D(camera);

  DrawLine3D(g0, g1, ApplicationState::getInstance()->holdingRotate ? GREEN : RED);
  DrawLine3D(g1, g2, ApplicationState::getInstance()->holdingRotate ? GREEN : RED);
  DrawLine3D(g2, g3, ApplicationState::getInstance()->holdingRotate ? GREEN : RED);
  DrawLine3D(g3, g0, ApplicationState::getInstance()->holdingRotate ? GREEN : RED);

  // DrawLine3D(lastRay.position, Vector3Add(lastRay.position, Vector3Scale(lastRay.direction, lastDist)), BLUE);

  if (scene) {
    for (size_t i = 0; i < scene->nBodies(); ++i) {
      scene->getBody(i)->draw();
    }

    for (size_t i = 0; i < scene->nPoints(); ++i) {
      std::shared_ptr<RasterVertex> v = scene->getPoint(i);
      if (hoveredId == v->id) {
        v->color = v->activeColor();
      } else {
        v->color = v->passiveColor();
      }
      v->draw();
    }

    for (size_t i = 0; i < scene->nShapes(); ++i) {
      scene->getShape(i)->draw();
      if (hoveredId == scene->getShape(i)->id) {
        scene->getShape(i)->color = scene->getShape(i)->activeColor();
      } else {
        scene->getShape(i)->color = scene->getShape(i)->passiveColor();
      }
    }
  }

  EndMode3D();
}

void Viewport::receiveMousePos(Vector2 mousePos) {
  if (ApplicationState::getInstance()->holdingRotate) {
    Vector2 diff = Vector2Subtract(mousePos, lastMousePos);

    double azimuthalDiff = diff.x * cameraSensetivity;
    double inclinationDiff = -diff.y * cameraSensetivity;

    double cameraAzimuthal = std::acos(camera.position.x / std::sqrt(camera.position.x * camera.position.x + camera.position.z * camera.position.z));
    if (camera.position.z < 0.0) {
      cameraAzimuthal = -cameraAzimuthal;
    }
    double cameraInclination = std::acos(camera.position.y / cameraRadius);

    camera.position.x = cameraRadius * std::cos(cameraAzimuthal + azimuthalDiff) * std::sin(cameraInclination + inclinationDiff);
    camera.position.z = cameraRadius * std::sin(cameraAzimuthal + azimuthalDiff) * std::sin(cameraInclination + inclinationDiff);
    camera.position.y = cameraRadius * std::cos(cameraInclination + inclinationDiff);
  }

  Ray ray = getNonOffsetMouseRay(mousePos);
  RayCollision groundHitInfo = GetRayCollisionQuad(ray, g0, g1, g2, g3);
  lastDist = groundHitInfo.distance;
  if (groundHitInfo.hit) {
    std::optional<size_t> maybeId = ApplicationState::getInstance()
      ->occtScene->idContainingPoint(
        groundHitInfo.point.x, groundHitInfo.point.y, groundHitInfo.point.z
      );
    if (maybeId.has_value()) {
      hoveredId = maybeId.value();
    } else  {
      hoveredId = -1;
    }
  }

  if (scene->nPoints() > 0) {
    std::optional<std::shared_ptr<RasterVertex>> maybeClosest = scene->queryVertex(ray, ApplicationState::getInstance()->selectionThreshold);

    if (maybeClosest.has_value()) {
      hoveredId = maybeClosest.value()->id;
    }
  }

  lastMousePos = mousePos;
}

void Viewport::receiveMouseDown(Vector2 mousePos) {
  RayCollision collision = { 0 };
  collision.distance = FLT_MAX;
  collision.hit = false;

  Ray ray = getNonOffsetMouseRay(mousePos);
  lastRay = ray;

  // Maybe we should attempt to raycast in the scene itself rather than the viewport?
  RayCollision groundHitInfo = GetRayCollisionQuad(ray, g0, g1, g2, g3);
  lastDist = groundHitInfo.distance;

  if ((groundHitInfo.hit && groundHitInfo.distance < collision.distance)) {
    EventQueue::getInstance()->postEvent(groundPlaneHit {
        groundHitInfo.point.x,
        groundHitInfo.point.y,
        groundHitInfo.point.z,
        ray
    });
  }
}

void Viewport::receiveMouseUp(Vector2 mousePos) {
}

void Viewport::receiveMouseWheel(Vector2 mousePos, float movement) {
}

Vector2 Viewport::getSize() {
  return Vector2 { areaSreenRect->width, areaSreenRect->height };
}

void Viewport::setScene(std::shared_ptr<Scene> scene) {
  this->scene = scene;
}

void Viewport::setAreaPointers(
  Rectangle* screenRect,
  Vector2* screenPos,
  RenderTexture* texture
) {
  areaSreenRect = screenRect;
  areaScreenPos = screenPos;
  areaTexture = texture;
}

Ray Viewport::getNonOffsetMouseRay(Vector2 mousePos) {
  Vector2 offset = { 0.0, 0.0 };
  if (areaSreenRect != nullptr && areaScreenPos != nullptr && areaTexture != nullptr) {
    offset.x = (areaSreenRect->width - areaTexture->texture.width) / 2.0;
    offset.y = (areaSreenRect->height - areaTexture->texture.height) / 2.0;
  }

  Ray ray = { 0 };
  ray = GetMouseRay(Vector2Subtract(mousePos, offset), camera);

  return ray;
}

}
