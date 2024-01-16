#pragma once

#include "quoll/asset/Asset.h"
#include "quoll/signals/SignalSlot.h"

#include "LuaHeaders.h"
#include "LuaScriptInputVariable.h"
#include "DeferredLoader.h"

namespace quoll {

struct LuaScript {
  LuaScriptAssetHandle handle = LuaScriptAssetHandle::Null;

  bool started = false;

  lua_State *state;

  std::unordered_map<String, LuaScriptInputVariable> variables;

  std::vector<SignalSlot> signalSlots;

  lua::DeferredLoader loader;
};

} // namespace quoll
