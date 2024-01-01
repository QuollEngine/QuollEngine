#pragma once

#include "quoll/lua-scripting/LuaUserTypeBase.h"

namespace quoll {

/**
 * @brief Lua interface for local transform
 */
class TransformLuaTable {
public:
  /**
   * @brief Create transform
   *
   * @param entity Entity
   * @param scriptGlobals Script globals
   */
  TransformLuaTable(Entity entity, ScriptGlobals scriptGlobals);

  /**
   * @brief Get local scale
   *
   * @return Local scale
   */
  std::tuple<f32, f32, f32> getScale();

  /**
   * @brief Set local scale
   *
   * @param x Local scale in x axis
   * @param y Local scale in y axis
   * @param z Local scale in z axis
   */
  void setScale(f32 x, f32 y, f32 z);

  /**
   * @brief Get local position
   *
   * @return Local position
   */
  std::tuple<f32, f32, f32> getPosition();

  /**
   * @brief Set local position
   *
   * @param x Local position in x axis
   * @param y Local position in y axis
   * @param z Local position in z axis
   */
  void setPosition(f32 x, f32 y, f32 z);

  /**
   * @brief Get rotation
   *
   * @return Local rotation in euler angles
   */
  std::tuple<f32, f32, f32> getRotation();

  /**
   * @brief Set local rotation
   *
   * @param x Local rotation as euler angles in x axis
   * @param y Local rotation as euler angles in y axis
   * @param z Local rotation as euler angles in z axis
   */
  void setRotation(f32 x, f32 y, f32 z);

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
  static void create(sol::usertype<TransformLuaTable> usertype,
                     sol::state_view state);

  /**
   * @brief Get component name in scripts
   *
   * @return Component name
   */
  static const String getName() { return "localTransform"; }

private:
  Entity mEntity;
  ScriptGlobals mScriptGlobals;
};

} // namespace quoll
