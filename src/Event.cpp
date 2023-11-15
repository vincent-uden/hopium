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
}

AppEvent EventQueue::pop() {
  AppEvent event = eventQueue.front();
  eventQueue.pop();

  return event;
}

bool EventQueue::empty() {
  return eventQueue.empty();
}
