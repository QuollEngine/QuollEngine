#pragma once

#include "quoll/lua-scripting/LuaUserTypeBase.h"

namespace quoll {

/**
 * @brief Lua scripting interface for rigid body
 */
class RigidBodyLuaTable {
public:
  /**
   * @brief Create rigid body table
   *
   * @param entity Entity
   * @param scriptGlobals Script globals
   */
  RigidBodyLuaTable(Entity entity, ScriptGlobals scriptGlobals);

  /**
   * @brief Set default params
   */
  void setDefaultParams();

  /**
   * @brief Get rigid body type
   *
   * @return Rigid body type
   */
  sol_maybe<u32> getType();

  /**
   * @brief Get mass
   *
   * @return Mass
   */
  sol_maybe<f32> getMass();

  /**
   * @brief Set mass
   *
   * @param mass Mass
   */
  void setMass(f32 mass);

  /**
   * @brief Get inertia
   *
   * @return Inertia
   */
  std::tuple<sol_maybe<f32>, sol_maybe<f32>, sol_maybe<f32>> getInertia();

  /**
   * @brief Set inertia
   *
   * @param x Inertia in x axis
   * @param y Inertia in y axis
   * @param z Inertia in z axis
   */
  void setInertia(f32 x, f32 y, f32 z);

  /**
   * @brief Check if gravity is applied
   *
   * @retval true Gravity is applied
   * @retval false Gravity is not applied
   */
  sol_maybe<bool> isGravityApplied();

  /**
   * @brief Apply gravity
   *
   * @param apply Should apply gravity
   */
  void applyGravity(bool apply);

  /**
   * @brief Apply force
   *
   * @param x Force in x axis
   * @param y Force in y axis
   * @param z Force in z axis
   */
  void applyForce(f32 x, f32 y, f32 z);

  /**
   * @brief Apply impulse
   *
   * @param x Impulse in x axis
   * @param y Impulse in y axis
   * @param z Impulse in z axis
   */
  void applyImpulse(f32 x, f32 y, f32 z);

  /**
   * @brief Apply torque
   *
   * @param x Torque in x axis
   * @param y Torque in y axis
   * @param z Torque in z axis
   */
  void applyTorque(f32 x, f32 y, f32 z);

  /**
   * @brief Clear force and torque
   */
  void clear();

  /**
   * @brief Delete component
   */
  void deleteThis();

  /**
   * @brief Create user type
   *
   * @param usertype User type
   * @param state Sol state
   */
  static void create(sol::usertype<RigidBodyLuaTable> usertype,
                     sol::state_view state);

  /**
   * @brief Get component name in scripts
   *
   * @return Component name
   */
  static const String getName() { return "rigidBody"; }

private:
  Entity mEntity;
  ScriptGlobals mScriptGlobals;
};

} // namespace quoll
