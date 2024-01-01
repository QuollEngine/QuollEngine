#pragma once

#include "quoll/lua-scripting/LuaUserTypeBase.h"
#include "AnimationStateLuaTable.h"

namespace quoll {

/**
 * @brief Lua table for animator
 */
class AnimatorLuaTable {
public:
  /**
   * @brief Create animator table
   *
   * @param entity Entity
   * @param scriptGlobals Script globals
   */
  AnimatorLuaTable(Entity entity, ScriptGlobals scriptGlobals);

  /**
   * @brief Get normalized time
   *
   * @return Normalized time or nil
   */
  sol_maybe<f32> getNormalizedTime();

  /**
   * @brief Get current animation state
   *
   * @return Animation state or nil
   */
  sol_maybe<AnimationStateLuaTable> getCurrentState();

  /**
   * @brief Trigger animation event
   *
   * @param event Event to trigger
   */
  void trigger(String event);

  /**
   * @brief Delete component
   */
  void deleteThis();

  /**
   * @brief Get component name in scripts
   *
   * @return Component name
   */
  static const String getName() { return "animator"; }

  /**
   * @brief Create user type
   *
   * @param usertype User type
   * @param state Sol state
   */
  static void create(sol::usertype<AnimatorLuaTable> usertype,
                     sol::state_view state);

private:
  Entity mEntity;
  ScriptGlobals mScriptGlobals;
};

} // namespace quoll
