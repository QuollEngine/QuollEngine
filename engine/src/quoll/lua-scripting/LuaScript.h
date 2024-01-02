#pragma once

#include "quoll/asset/Asset.h"
#include "quoll/signals/SignalSlot.h"

#include "LuaHeaders.h"
#include "LuaScriptInputVariable.h"
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
   * Signal slots
   */
  std::vector<SignalSlot> signalSlots;

  /**
   * Deferred loader
   */
  lua::DeferredLoader loader;
};

} // namespace quoll
