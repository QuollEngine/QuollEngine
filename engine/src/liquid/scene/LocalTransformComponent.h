#pragma once

#include "liquid/scripting/ComponentLuaInterface.h"

namespace liquid {

/**
 * @brief Local transform component
 */
struct LocalTransformComponent {
  struct LuaInterface;
  /**
   * Local position
   */
  glm::vec3 localPosition{0.0f};

  /**
   * Local rotation
   */
  glm::quat localRotation{1.0f, 0.0f, 0.0f, 0.0f};

  /**
   * Local scale
   */
  glm::vec3 localScale{1.0f};
};

/**
 * @brief Lua interface for local transform component
 */
struct LocalTransformComponent::LuaInterface
    : public ComponentLuaInterface<LocalTransformComponent::LuaInterface> {

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
  static constexpr std::array<InterfaceField, 4> fields{
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
