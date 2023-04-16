#pragma once

#include "liquid/scripting/ComponentLuaInterface.h"

namespace liquid {

/**
 * @brief Scripting interfaces for rigid body
 */
struct RigidBodyScriptingInterface {
public:
  class LuaInterface;
};

/**
 * @brief Lua scripting interface for rigid body
 */
class RigidBodyScriptingInterface::LuaInterface
    : public ComponentLuaInterface<RigidBodyScriptingInterface::LuaInterface> {
public:
  /**
   * @brief Set default params
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int setDefaultParams(void *state);

  /**
   * @brief Get mass
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int getMass(void *state);

  /**
   * @brief Set mass
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int setMass(void *state);

  /**
   * @brief Get inertia
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int getInertia(void *state);

  /**
   * @brief Set inertia
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int setInertia(void *state);

  /**
   * @brief Check if gravity is applied
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int isGravityApplied(void *state);

  /**
   * @brief Apply gravity
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int applyGravity(void *state);

  /**
   * @brief Apply force
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int applyForce(void *state);

  /**
   * @brief Apply torque
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int applyTorque(void *state);

  /**
   * @brief Clear force and torque
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int clear(void *state);

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
  static constexpr std::array<InterfaceField, 11> Fields{
      InterfaceField{"set_default_params", setDefaultParams},
      InterfaceField{"get_mass", getMass},
      InterfaceField{"set_mass", setMass},
      InterfaceField{"get_inertia", getInertia},
      InterfaceField{"set_inertia", setInertia},
      InterfaceField{"is_gravity_applied", isGravityApplied},
      InterfaceField{"apply_gravity", applyGravity},
      InterfaceField{"apply_force", applyForce},
      InterfaceField{"apply_torque", applyTorque},
      InterfaceField{"clear", clear},
      InterfaceField{"delete", deleteThis}};

  /**
   * @brief Get component name in scripts
   *
   * @return Component name
   */
  static const String getName() { return "rigid_body"; }
};

} // namespace liquid
