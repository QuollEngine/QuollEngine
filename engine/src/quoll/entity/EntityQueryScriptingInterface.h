#pragma once

#include "quoll/scripting/ComponentLuaInterface.h"

namespace quoll {

/**
 * @brief Scripting interfaces for entity query
 */
struct EntityQueryScriptingInterface {
public:
  class LuaInterface;
};

/**
 * @brief Lua scripting interface for entity query
 */
class EntityQueryScriptingInterface::LuaInterface
    : public ComponentLuaInterface<
          EntityQueryScriptingInterface::LuaInterface> {
public:
  /**
   * @brief Get first entity by name
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int getFirstEntityByName(void *state);

  /**
   * @brief Delete entity
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int deleteEntity(void *state);

  /**
   * @brief Interface fields
   */
  static constexpr std::array<InterfaceField, 2> Fields{
      InterfaceField{"get_first_entity_by_name", getFirstEntityByName},
      InterfaceField{"delete_entity", deleteEntity}};

  /**
   * @brief Get component name in scripts
   *
   * @return Component name
   */
  static const String getName() { return "entity_query"; }
};

} // namespace quoll
