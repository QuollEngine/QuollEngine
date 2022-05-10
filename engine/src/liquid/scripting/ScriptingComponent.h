#pragma once

#include "ScriptHandle.h"
#include "liquid/asset/Asset.h"
#include "liquid/events/EventObserver.h"

namespace liquid {

/**
 * @brief Scripting component
 *
 * Provides data about the current state
 * of the script
 */
struct ScriptingComponent {
  /**
   * Lua script handle
   */
  LuaScriptAssetHandle handle = LuaScriptAssetHandle::Invalid;

  /**
   * Script started
   */
  bool started = false;

  /**
   * Lua scope
   */
  void *scope = nullptr;

  /**
   * Collision start observer
   */
  EventObserverId onCollisionStart = EVENT_OBSERVER_MAX;

  /**
   * Collision end observer
   */
  EventObserverId onCollisionEnd = EVENT_OBSERVER_MAX;

  /**
   * Key press observer
   */
  EventObserverId onKeyPress = EVENT_OBSERVER_MAX;

  /**
   * Key release observer
   */
  EventObserverId onKeyRelease = EVENT_OBSERVER_MAX;
};

} // namespace liquid
