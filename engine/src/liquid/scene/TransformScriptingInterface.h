#pragma once

#include "liquid/scripting/ComponentLuaInterface.h"

namespace liquid {

/**
 * @brief Scripting interfaces for rigid body
 */
struct TransformScriptingInterface {
public:
  class LuaInterface;
};

/**
 * @brief Lua interface for local transform component
 */
class TransformScriptingInterface::LuaInterface
    : public ComponentLuaInterface<TransformScriptingInterface::LuaInterface> {
public:
  /**
   * @brief Get scale
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int getScale(void *state);

  /**
   * @brief Set scale
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int setScale(void *state);

  /**
   * @brief Get position
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int getPosition(void *state);

  /**
   * @brief Set position
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int setPosition(void *state);

  /**
   * @brief Interface fields
   */
  static constexpr std::array<InterfaceField, 4> Fields{
      InterfaceField{"get_scale", getScale},
      InterfaceField{"set_scale", setScale},
      InterfaceField{"get_position", getPosition},
      InterfaceField{"set_position", setPosition}};

  /**
   * @brief Get component name in scripts
   *
   * @return Component name
   */
  static const String getName() { return "local_transform"; }
};

} // namespace liquid
