#pragma once

#include "liquid/entity/Entity.h"
#include "liquid/entity/EntityDatabase.h"

namespace liquid {

/**
 * @brief Decorates scope with with globals
 */
class ScriptDecorator {
public:
  /**
   * @brief Attach to scope
   *
   * @param scope Lua scope
   * @param entity Entity
   * @param entityDatabase Entity database
   */
  void attachToScope(LuaScope &scope, Entity entity,
                     EntityDatabase &entityDatabase);

  /**
   * @brief Attach variable injectors
   *
   * @param scope Lua scope
   * @param variables Input variables
   */
  void attachVariableInjectors(
      LuaScope &scope,
      std::unordered_map<String, LuaScriptInputVariable> &variables);

  /**
   * @brief Remove variable injectors
   *
   * @param scope Lua scope
   */
  void removeVariableInjectors(LuaScope &scope);

  /**
   * @brief Create entity table for the given entity
   *
   * @param scope Lua scope
   * @param entity Entity
   */
  static void createEntityTable(LuaScope &scope, Entity entity);
};

} // namespace liquid
