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
   * @brief Interface fields
   */
  static constexpr std::array<InterfaceField, 2> fields{
      InterfaceField{"apply_force", applyForce},
      InterfaceField{"apply_torque", applyTorque}};

  /**
   * @brief Get component name in scripts
   *
   * @return Component name
   */
  static const String getName() { return "rigid_body"; }
};

} // namespace liquid
