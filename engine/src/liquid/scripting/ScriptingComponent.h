#pragma once

#include "ScriptHandle.h"
#include "liquid/asset/Asset.h"
#include "liquid/events/EventObserver.h"

namespace liquid {

struct ScriptingComponent {
  LuaScriptAssetHandle handle = LuaScriptAssetHandle::Invalid;
  bool started = false;
  void *scope = nullptr;

  EventObserverId onCollisionStart = EVENT_OBSERVER_MAX;
  EventObserverId onCollisionEnd = EVENT_OBSERVER_MAX;
  EventObserverId onKeyPress = EVENT_OBSERVER_MAX;
  EventObserverId onKeyRelease = EVENT_OBSERVER_MAX;
};

} // namespace liquid
