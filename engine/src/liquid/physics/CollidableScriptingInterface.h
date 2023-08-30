#pragma once

#include "liquid/scripting/ComponentLuaInterface.h"

namespace quoll {

/**
 * @brief Scripting interfaces for collidable
 */
struct CollidableScriptingInterface {
public:
  class LuaInterface;
};

/**
 * @brief Lua scripting interface for rigid body
 */
class CollidableScriptingInterface::LuaInterface
    : public ComponentLuaInterface<CollidableScriptingInterface::LuaInterface> {
public:
  /**
   * @brief Set default material
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int setDefaultMaterial(void *state);

  /**
   * @brief Get static friction
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int getStaticFriction(void *state);

  /**
   * @brief Set static friction
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int setStaticFriction(void *state);

  /**
   * @brief Get dynamic friction
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int getDynamicFriction(void *state);

  /**
   * @brief Set dynamic friction
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int setDynamicFriction(void *state);

  /**
   * @brief Get restitution
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int getRestitution(void *state);

  /**
   * @brief Set restitution
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int setRestitution(void *state);

  /**
   * @brief Set box geometry
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int setBoxGeometry(void *state);

  /**
   * @brief Set sphere geometry
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int setSphereGeometry(void *state);

  /**
   * @brief Set capsule geometry
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int setCapsuleGeometry(void *state);

  /**
   * @brief Set plane geometry
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int setPlaneGeometry(void *state);

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
  static constexpr std::array<InterfaceField, 12> Fields{
      InterfaceField{"set_default_material", setDefaultMaterial},
      InterfaceField{"get_static_friction", getStaticFriction},
      InterfaceField{"set_static_friction", setStaticFriction},
      InterfaceField{"get_dynamic_friction", getDynamicFriction},
      InterfaceField{"set_dynamic_friction", setDynamicFriction},
      InterfaceField{"get_restitution", getRestitution},
      InterfaceField{"set_restitution", setRestitution},
      InterfaceField{"set_box_geometry", setBoxGeometry},
      InterfaceField{"set_sphere_geometry", setSphereGeometry},
      InterfaceField{"set_capsule_geometry", setCapsuleGeometry},
      InterfaceField{"set_plane_geometry", setPlaneGeometry},
      InterfaceField{"delete", deleteThis}};

  /**
   * @brief Get component name in scripts
   *
   * @return Component name
   */
  static const String getName() { return "collidable"; }
};

} // namespace quoll
