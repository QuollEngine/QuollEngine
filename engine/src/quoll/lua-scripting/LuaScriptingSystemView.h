#pragma once

#include "quoll/entity/EntityDatabase.h"
#include "LuaScript.h"

namespace quoll {

struct LuaScriptingSystemView {
  EntityDatabaseObserver<LuaScript> scriptRemoveObserver;
};

} // namespace quoll
