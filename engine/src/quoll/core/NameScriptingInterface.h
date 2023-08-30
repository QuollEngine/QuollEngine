#pragma once

#include "quoll/scripting/ComponentLuaInterface.h"

namespace quoll {

/**
 * @brief Scripting interfaces for name component
 */
struct NameScriptingInterface {
  class LuaInterface;
};

/**
 * @brief Lua interface for name component
 */
class NameScriptingInterface::LuaInterface
    : public ComponentLuaInterface<NameScriptingInterface::LuaInterface> {
public:
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
   * @brief Delete component
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int deleteThis(void *state);

  /**
   * @brief Interface fields
   */
  static constexpr std::array<InterfaceField, 3> Fields{
      InterfaceField{"get", get}, InterfaceField{"set", set},
      InterfaceField{"delete", deleteThis}};

  /**
   * @brief Get component name in scripts
   *
   * @return Component name
   */
  static const String getName() { return "name"; }
};

} // namespace quoll
