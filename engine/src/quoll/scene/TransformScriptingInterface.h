#pragma once

#include "quoll/scripting/ComponentLuaInterface.h"

namespace quoll {

/**
 * @brief Scripting interfaces for local transform
 */
struct TransformScriptingInterface {
public:
  class LuaInterface;
};

/**
 * @brief Lua interface for local transform
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
   * @brief Get rotation
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int getRotation(void *state);

  /**
   * @brief Set rotation
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int setRotation(void *state);

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
  static constexpr std::array<InterfaceField, 7> Fields{
      InterfaceField{"get_scale", getScale},
      InterfaceField{"set_scale", setScale},
      InterfaceField{"get_position", getPosition},
      InterfaceField{"set_position", setPosition},
      InterfaceField{"get_rotation", getRotation},
      InterfaceField{"set_rotation", setRotation},
      InterfaceField{"delete", deleteThis}};

  /**
   * @brief Get component name in scripts
   *
   * @return Component name
   */
  static const String getName() { return "local_transform"; }
};

} // namespace quoll
