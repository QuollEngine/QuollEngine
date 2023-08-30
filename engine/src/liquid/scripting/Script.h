#pragma once

#include "ScriptHandle.h"
#include "liquid/asset/Asset.h"
#include "liquid/events/EventObserver.h"
#include "liquid/scripting/LuaScriptInputVariable.h"

#include "LuaScope.h"

namespace quoll {

/**
 * @brief Scripting component
 *
 * Provides data about the current state
 * of the script
 */
struct Script {
  /**
   * Lua script handle
   */
  LuaScriptAssetHandle handle = LuaScriptAssetHandle::Null;

  /**
   * Script started
   */
  bool started = false;

  /**
   * Lua scope
   */
  LuaScope scope;

  /**
   * Input variables
   */
  std::unordered_map<String, LuaScriptInputVariable> variables;

  /**
   * Collision start observer
   */
  EventObserverId onCollisionStart = EventObserverMax;

  /**
   * Collision end observer
   */
  EventObserverId onCollisionEnd = EventObserverMax;

  /**
   * Key press observer
   */
  EventObserverId onKeyPress = EventObserverMax;

  /**
   * Key release observer
   */
  EventObserverId onKeyRelease = EventObserverMax;
};

} // namespace quoll
