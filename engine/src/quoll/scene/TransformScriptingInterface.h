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
class TransformScriptingInterface::LuaInterface {
public:
  /**
   * @brief Create transform
   *
   * @param entity Entity
   * @param scriptGlobals Script globals
   */
  LuaInterface(Entity entity, ScriptGlobals scriptGlobals);

  /**
   * @brief Get local scale
   *
   * @return Local scale
   */
  std::tuple<float, float, float> getScale();

  /**
   * @brief Set local scale
   *
   * @param x Local scale in x axis
   * @param y Local scale in y axis
   * @param z Local scale in z axis
   */
  void setScale(float x, float y, float z);

  /**
   * @brief Get local position
   *
   * @return Local position
   */
  std::tuple<float, float, float> getPosition();

  /**
   * @brief Set local position
   *
   * @param x Local position in x axis
   * @param y Local position in y axis
   * @param z Local position in z axis
   */
  void setPosition(float x, float y, float z);

  /**
   * @brief Get rotation
   *
   * @return Local rotation in euler angles
   */
  std::tuple<float, float, float> getRotation();

  /**
   * @brief Set local rotation
   *
   * @param x Local rotation as euler angles in x axis
   * @param y Local rotation as euler angles in y axis
   * @param z Local rotation as euler angles in z axis
   */
  void setRotation(float x, float y, float z);

  /**
   * @brief Delete component
   */
  void deleteThis();

  /**
   * @brief Create user type
   *
   * @param usertype User type
   */
  static void
  create(sol::usertype<TransformScriptingInterface::LuaInterface> usertype);

  /**
   * @brief Get component name in scripts
   *
   * @return Component name
   */
  static const String getName() { return "local_transform"; }

private:
  Entity mEntity;
  ScriptGlobals mScriptGlobals;
};

} // namespace quoll
