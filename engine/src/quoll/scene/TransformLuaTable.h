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
   * @brief Get local position
   *
   * @return Local position
   */
  std::reference_wrapper<glm::vec3> getPosition();

  /**
   * @brief Set local position
   *
   * @param position Local position
   */
  void setPosition(glm::vec3 position);

  /**
   * @brief Get rotation
   *
   * @return Local rotation
   */
  std::reference_wrapper<glm::quat> getRotation();

  /**
   * @brief Set local rotation
   *
   * @param rotation Local rotation
   */
  void setRotation(glm::quat rotation);

  /**
   * @brief Get local scale
   *
   * @return Local scale
   */
  std::reference_wrapper<glm::vec3> getScale();

  /**
   * @brief Set local scale
   *
   * @param scale Local scale
   */
  void setScale(glm::vec3 scale);

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
