#include "Event.h"

EventQueue* EventQueue::instance = nullptr;

EventQueue::EventQueue() {
  if (instance == nullptr) {
    instance = this;
  } else {
    throw std::runtime_error("Event queue already exists");
  }
}

EventQueue::~EventQueue() {
}

EventQueue* EventQueue::getInstance() {
  if (instance == nullptr) {
    instance = new EventQueue();
  }
  return instance;
}

void EventQueue::postEvent(AppEvent event) {
  eventQueue.push(event);
  history.push_back(event);
}

void EventQueue::resetHistoryIndex() {
  historyIndex = -1;
}

AppEvent EventQueue::pop() {
  AppEvent event = eventQueue.front();
  eventQueue.pop();

  return event;
}

std::optional<AppEvent> EventQueue::getNextHistoryEvent() {
  if (historyIndex < history.size()) {
    return std::optional<AppEvent>(history[historyIndex++]);
  } else {
    return std::nullopt;
  }
}

bool EventQueue::empty() {
  return eventQueue.empty();
}

json EventQueue::serializeHistory() {
  json out;

  for (const AppEvent& e: history) {
    json data;
    data["type"] = e.index();
    if (std::holds_alternative<groundPlaneHit>(e)) {
      groundPlaneHit hit = std::get<groundPlaneHit>(e);
      data["data"]["x"] = hit.x;
      data["data"]["y"] = hit.y;
      data["data"]["z"] = hit.z;
      data["data"]["ray"]["position"]["x"] = hit.ray.position.x;
      data["data"]["ray"]["position"]["y"] = hit.ray.position.y;
      data["data"]["ray"]["position"]["z"] = hit.ray.position.z;
      data["data"]["ray"]["direction"]["x"] = hit.ray.direction.x;
      data["data"]["ray"]["direction"]["y"] = hit.ray.direction.y;
      data["data"]["ray"]["direction"]["z"] = hit.ray.direction.z;
    }

    out["history"].push_back(data);
  }

  return out;
}

void EventQueue::deserializeHistory(json state) {
  history.clear();
  resetHistoryIndex();

  for (const json& e: state["history"]) {
    size_t index = e["type"];
    AppEvent event = APP_EVENTS[index];
    if (std::holds_alternative<groundPlaneHit>(event)) {
      groundPlaneHit hit = std::get<groundPlaneHit>(event);
      hit.x = e["data"]["x"];
      hit.y = e["data"]["y"];
      hit.z = e["data"]["z"];
      hit.ray.position.x = e["data"]["ray"]["position"]["x"];
      hit.ray.position.y = e["data"]["ray"]["position"]["y"];
      hit.ray.position.z = e["data"]["ray"]["position"]["z"];
      hit.ray.direction.x = e["data"]["ray"]["direction"]["x"];
      hit.ray.direction.y = e["data"]["ray"]["direction"]["y"];
      hit.ray.direction.z = e["data"]["ray"]["direction"]["z"];
      event = hit;
    }

    history.push_back(event);
  }
}
