#pragma once

#include "liquid/entity/Entity.h"
#include "liquid/entity/EntityDatabase.h"

namespace liquid {

/**
 * @brief Decorates scope with entity system
 */
class EntityDecorator {
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
};

} // namespace liquid
