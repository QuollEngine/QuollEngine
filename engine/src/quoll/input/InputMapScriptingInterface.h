#pragma once

#include "quoll/scripting/ComponentLuaInterface.h"

namespace quoll {

/**
 * @brief Scripting interfaces for input map
 */
struct InputMapScriptingInterface {
public:
  class LuaInterface;
};

/**
 * @brief Lua scripting interface for input map
 */
class InputMapScriptingInterface::LuaInterface {
public:
  /**
   * @brief Create input map table
   *
   * @param entity Entity
   * @param scriptGlobals Script globals
   */
  LuaInterface(Entity entity, ScriptGlobals scriptGlobals);

  /**
   * @brief Get command
   *
   * @param name Command name
   * @return Command id
   */
  sol_maybe<size_t> getCommand(String name);

  /**
   * @brief Check if command is pressed
   *
   * @param command Command id
   * @return Command boolean value
   */
  sol_maybe<bool> getCommandValueBoolean(size_t command);

  /**
   * @brief Get axis 2d command value
   *
   * @param command Command id
   * @return Command 2d axis value
   */
  std::tuple<sol_maybe<float>, sol_maybe<float>>
  getCommandValueAxis2d(size_t command);

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
   */
  static void
  create(sol::usertype<InputMapScriptingInterface::LuaInterface> usertype);

private:
  Entity mEntity;
  ScriptGlobals mScriptGlobals;
};

} // namespace quoll
