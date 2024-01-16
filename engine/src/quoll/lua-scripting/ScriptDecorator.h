#pragma once

#include "quoll/asset/AssetRegistry.h"
#include "quoll/entity/Entity.h"
#include "quoll/entity/EntityDatabase.h"
#include "quoll/physics/PhysicsSystem.h"
#include "LuaScript.h"
#include "ScriptGlobals.h"

namespace quoll::lua {

/**
 * @brief Decorates Lua scope with with globals
 */
class ScriptDecorator {
public:
  void attachToScope(sol::state_view state, Entity entity,
                     ScriptGlobals scriptGlobals);

  void attachVariableInjectors(
      sol::state_view state,
      std::unordered_map<String, LuaScriptInputVariable> &variables);

  void removeVariableInjectors(sol::state_view state);
};

} // namespace quoll::lua
