#pragma once

#include "quoll/scripting/ComponentLuaInterface.h"

namespace quoll {

/**
 * @brief Scripting interfaces for input map
 */
struct InputMapScriptingInterface {
public:
  class LuaInterface;
};

/**
 * @brief Lua scripting interface for input map
 */
class InputMapScriptingInterface::LuaInterface
    : public ComponentLuaInterface<InputMapScriptingInterface::LuaInterface> {
public:
  /**
   * @brief Get command
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int getCommand(void *state);

  /**
   * @brief Check if command is pressed
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int getCommandValueBoolean(void *state);

  /**
   * @brief Set static friction
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int getCommandValueAxis2d(void *state);

  /**
   * @brief Interface fields
   */
  static constexpr std::array<InterfaceField, 4> Fields{
      InterfaceField{"get_command", getCommand},
      InterfaceField{"get_value_boolean", getCommandValueBoolean},
      InterfaceField{"get_value_axis_2d", getCommandValueAxis2d},
      InterfaceField{"is_pressed", getCommandValueBoolean}};

  /**
   * @brief Get component name in scripts
   *
   * @return Component name
   */
  static const String getName() { return "input"; }
};

} // namespace quoll
