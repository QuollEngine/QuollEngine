#pragma once

#include "quoll/lua-scripting/LuaUserTypeBase.h"

namespace quoll {

/**
 * @brief Lua scripting interface for input map
 */
class InputMapLuaTable {
public:
  /**
   * @brief Create input map table
   *
   * @param entity Entity
   * @param scriptGlobals Script globals
   */
  InputMapLuaTable(Entity entity, ScriptGlobals scriptGlobals);

  /**
   * @brief Get command
   *
   * @param name Command name
   * @return Command id
   */
  sol_maybe<usize> getCommand(String name);

  /**
   * @brief Check if command is pressed
   *
   * @param command Command id
   * @return Command boolean value
   */
  sol_maybe<bool> getCommandValueBoolean(usize command);

  /**
   * @brief Get axis 2d command value
   *
   * @param command Command id
   * @return Command 2d axis value
   */
  std::tuple<sol_maybe<f32>, sol_maybe<f32>>
  getCommandValueAxis2d(usize command);

  /**
   * @brief Set scheme
   *
   * @param name Scheme name
   */
  void setScheme(String name);

  /**
   * @brief Get component name in scripts
   *
   * @return Component name
   */
  static const String getName() { return "input"; }

  /**
   * @brief Create user type
   *
   * @param usertype User type
   * @param state Sol state
   */
  static void create(sol::usertype<InputMapLuaTable> usertype,
                     sol::state_view state);

private:
  Entity mEntity;
  ScriptGlobals mScriptGlobals;
};

} // namespace quoll
