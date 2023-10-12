#pragma once

#include "quoll/scripting/ComponentLuaInterface.h"
#include "CollisionHit.h"

namespace quoll {

/**
 * @brief Lua scripting interface for collidable
 */
class CollidableLuaTable {
public:
  /**
   * @brief Create collidable table
   *
   * @param entity Entity
   * @param scriptGlobals Script globals
   */
  CollidableLuaTable(Entity entity, ScriptGlobals scriptGlobals);

  /**
   * @brief Set default material
   */
  void setDefaultMaterial();

  /**
   * @brief Get static friction
   *
   * @return Static friction
   */
  sol_maybe<float> getStaticFriction();

  /**
   * @brief Set static friction
   *
   * @param staticFriction Static friction
   */
  void setStaticFriction(float staticFriction);

  /**
   * @brief Get dynamic friction
   *
   * @return Dynamic friction
   */
  sol_maybe<float> getDynamicFriction();

  /**
   * @brief Set dynamic friction
   *
   * @param dynamicFriction Dynamic friction
   */
  void setDynamicFriction(float dynamicFriction);

  /**
   * @brief Get restitution
   *
   * @return Restitution
   */
  sol_maybe<float> getRestitution();

  /**
   * @brief Set restitution
   *
   * @param restitution Restitution
   */
  void setRestitution(float restitution);

  /**
   * @brief Set box geometry
   *
   * @param hx Half extent in x axis
   * @param hy Half extent in y axis
   * @param hz Half extent in z axis
   */
  void setBoxGeometry(float hx, float hy, float hz);

  /**
   * @brief Set sphere geometry
   *
   * @param radius Sphere radius
   */
  void setSphereGeometry(float radius);

  /**
   * @brief Set capsule geometry
   *
   * @param radius Capsule radius
   * @param halfHeight Capsule half height
   */
  void setCapsuleGeometry(float radius, float halfHeight);

  /**
   * @brief Set plane geometry
   */
  void setPlaneGeometry();

  /**
   * @brief Sweep test
   *
   * @param dx Sweep direction in x axis
   * @param dy Sweep direction in y axis
   * @param dz Sweep direction in z axis
   * @param distance Sweep distance
   * @return Collision hit
   */
  std::tuple<bool, sol_maybe<CollisionHit>> sweep(float dx, float dy, float dz,
                                                  float distance);

  /**
   * @brief Delete component
   */
  void deleteThis();

  /**
   * @brief Create user type
   *
   * @param usertype User type
   */
  static void create(sol::usertype<CollidableLuaTable> usertype);

  /**
   * @brief Get component name in scripts
   *
   * @return Component name
   */
  static const String getName() { return "collidable"; }

private:
  Entity mEntity;
  ScriptGlobals mScriptGlobals;
};

} // namespace quoll
