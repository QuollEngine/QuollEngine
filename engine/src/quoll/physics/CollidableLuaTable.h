#pragma once

#include "quoll/lua-scripting/LuaUserTypeBase.h"
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
  sol_maybe<f32> getStaticFriction();

  /**
   * @brief Set static friction
   *
   * @param staticFriction Static friction
   */
  void setStaticFriction(f32 staticFriction);

  /**
   * @brief Get dynamic friction
   *
   * @return Dynamic friction
   */
  sol_maybe<f32> getDynamicFriction();

  /**
   * @brief Set dynamic friction
   *
   * @param dynamicFriction Dynamic friction
   */
  void setDynamicFriction(f32 dynamicFriction);

  /**
   * @brief Get restitution
   *
   * @return Restitution
   */
  sol_maybe<f32> getRestitution();

  /**
   * @brief Set restitution
   *
   * @param restitution Restitution
   */
  void setRestitution(f32 restitution);

  /**
   * @brief Set box geometry
   *
   * @param hx Half extent in x axis
   * @param hy Half extent in y axis
   * @param hz Half extent in z axis
   */
  void setBoxGeometry(f32 hx, f32 hy, f32 hz);

  /**
   * @brief Set sphere geometry
   *
   * @param radius Sphere radius
   */
  void setSphereGeometry(f32 radius);

  /**
   * @brief Set capsule geometry
   *
   * @param radius Capsule radius
   * @param halfHeight Capsule half height
   */
  void setCapsuleGeometry(f32 radius, f32 halfHeight);

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
  std::tuple<bool, sol_maybe<CollisionHit>> sweep(f32 dx, f32 dy, f32 dz,
                                                  f32 distance);

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
