#pragma once

#include "liquid/scripting/ComponentLuaInterface.h"

namespace liquid {

/**
 * @brief Scripting interfaces for audio
 */
struct EntityQueryScriptingInterface {
public:
  class LuaInterface;
};

/**
 * @brief Lua scripting interface for Audio
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
   * @brief Interface fields
   */
  static constexpr std::array<InterfaceField, 1> fields{
      InterfaceField{"get_first_entity_by_name", getFirstEntityByName}};

  /**
   * @brief Get component name in scripts
   *
   * @return Component name
   */
  static const String getName() { return "entity_query"; }
};

} // namespace liquid
