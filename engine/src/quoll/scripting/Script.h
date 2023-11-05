#pragma once

#include "ScriptHandle.h"
#include "quoll/asset/Asset.h"
#include "quoll/events/EventObserver.h"
#include "quoll/scripting/LuaScriptInputVariable.h"
#include "LuaHeaders.h"
#include "ScriptSignalSlot.h"

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
  std::vector<ScriptSignalSlot> signalSlots;
};

} // namespace quoll
