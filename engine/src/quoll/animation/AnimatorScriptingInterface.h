#pragma once

#include "quoll/scripting/ComponentLuaInterface.h"

namespace quoll {

/**
 * @brief Scripting interfaces for animator
 */
struct AnimatorScriptingInterface {
public:
  class LuaInterface;
};

/**
 * @brief Lua scripting interface for animator
 */
class AnimatorScriptingInterface::LuaInterface {
public:
  /**
   * @brief Create animator table
   *
   * @param entity Entity
   * @param scriptGlobals Script globals
   */
  LuaInterface(Entity entity, ScriptGlobals scriptGlobals);

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
  static void
  create(sol::usertype<AnimatorScriptingInterface::LuaInterface> usertype);

private:
  Entity mEntity;
  ScriptGlobals mScriptGlobals;
};

} // namespace quoll
