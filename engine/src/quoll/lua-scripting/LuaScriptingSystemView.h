#pragma once

#include "quoll/entity/EntityDatabase.h"
#include "LuaScript.h"

namespace quoll {

struct LuaScriptingSystemView {
  flecs::query<LuaScript> queryScripts;
};

} // namespace quoll
