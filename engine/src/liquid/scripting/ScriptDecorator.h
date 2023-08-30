#pragma once

#include "liquid/entity/Entity.h"
#include "liquid/entity/EntityDatabase.h"
#include "liquid/asset/AssetRegistry.h"

namespace quoll {

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
   * @param assetRegistry Asset registry
   */
  void attachToScope(LuaScope &scope, Entity entity,
                     EntityDatabase &entityDatabase,
                     AssetRegistry &assetRegistry);

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

} // namespace quoll
