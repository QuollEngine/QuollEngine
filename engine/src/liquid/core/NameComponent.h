#pragma once

#include "liquid/scripting/ComponentLuaInterface.h"

namespace liquid {

/**
 * @brief Name component
 */
struct NameComponent {
  struct LuaInterface;

  /**
   * Entity name
   */
  String name;
};

/**
 * @brief Lua interface for name component
 */
struct NameComponent::LuaInterface
    : public ComponentLuaInterface<NameComponent::LuaInterface> {

  /**
   * @brief Get name
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int get(void *state);

  /**
   * @brief Set name
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int set(void *state);

  /**
   * @brief Interface fields
   */
  static constexpr std::array<InterfaceField, 2> fields{
      InterfaceField{"get", get}, InterfaceField{"set", set}};

  /**
   * @brief Get component name in scripts
   *
   * @return Component name
   */
  static const String getName() { return "name"; }
};

} // namespace liquid
