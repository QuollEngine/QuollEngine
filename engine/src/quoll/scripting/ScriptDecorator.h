#pragma once

#include "quoll/entity/Entity.h"
#include "quoll/entity/EntityDatabase.h"
#include "quoll/asset/AssetRegistry.h"

#include "Script.h"
#include "quoll/physics/PhysicsSystem.h"

#include "ScriptGlobals.h"

namespace quoll {

/**
 * @brief Decorates scope with with globals
 */
class ScriptDecorator {
public:
  /**
   * @brief Attach to scope
   *
   * @param state Sol state
   * @param entity Entity
   * @param scriptGlobals Script globals
   */
  void attachToScope(sol::state_view state, Entity entity,
                     ScriptGlobals &scriptGlobals);

  /**
   * @brief Attach variable injectors
   *
   * @param state Sol state
   * @param variables Input variables
   */
  void attachVariableInjectors(
      sol::state_view state,
      std::unordered_map<String, LuaScriptInputVariable> &variables);

  /**
   * @brief Remove variable injectors
   *
   * @param state Sol state
   */
  void removeVariableInjectors(sol::state_view state);
};

} // namespace quoll
