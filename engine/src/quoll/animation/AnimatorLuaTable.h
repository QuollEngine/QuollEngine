#pragma once

#include "quoll/scripting/ComponentLuaInterface.h"

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
   */
  static void create(sol::usertype<AnimatorLuaTable> usertype);

private:
  Entity mEntity;
  ScriptGlobals mScriptGlobals;
};

} // namespace quoll
