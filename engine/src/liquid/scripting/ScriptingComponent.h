#pragma once

#include "ScriptHandle.h"
#include "liquid/events/EventObserver.h"

namespace liquid {

struct ScriptingComponent {
  ScriptHandle handle{0};
  bool started = false;
  void *scope = nullptr;

  EventObserverId onCollisionStart = EVENT_OBSERVER_MAX;
  EventObserverId onCollisionEnd = EVENT_OBSERVER_MAX;
};

} // namespace liquid
