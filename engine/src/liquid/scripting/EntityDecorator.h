#pragma once

#include "liquid/entity/Entity.h"
#include "liquid/entity/EntityContext.h"

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
   * @param entityContext Entity context
   */
  void attachToScope(LuaScope &scope, Entity entity,
                     EntityContext &entityContext);
};

} // namespace liquid
