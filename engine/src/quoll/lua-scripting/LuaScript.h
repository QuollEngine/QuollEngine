#pragma once

#include "quoll/asset/AssetRef.h"
#include "quoll/signals/SignalSlot.h"
#include "DeferredLoader.h"
#include "LuaHeaders.h"
#include "LuaScriptAsset.h"
#include "LuaScriptInputVariable.h"

namespace quoll {

struct LuaScript {
  AssetRef<LuaScriptAsset> asset;

  bool started = false;

  lua_State *state;

  std::unordered_map<String, LuaScriptInputVariable> variables;

  std::vector<SignalSlot> signalSlots;

  lua::DeferredLoader loader;
};

} // namespace quoll
