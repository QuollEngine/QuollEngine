#pragma once

#include "quoll/asset/Asset.h"
#include "quoll/events/EventObserver.h"

#include "LuaHeaders.h"
#include "LuaScriptInputVariable.h"
#include "ScriptSignalSlot.h"
#include "DeferredLoader.h"

namespace quoll {

/**
 * @brief Lua script component
 *
 * Provides data about the current state
 * of the Lua script
 */
struct LuaScript {
  /**
   * Lua script handle
   */
  LuaScriptAssetHandle handle = LuaScriptAssetHandle::Null;

  /**
   * Script started
   */
  bool started = false;

  /**
   * Lua state
   */
  lua_State *state;

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

  /**
   * Signal slots
   */
  std::vector<lua::ScriptSignalSlot> signalSlots;

  /**
   * Deferred loader
   */
  lua::DeferredLoader loader;
};

} // namespace quoll
