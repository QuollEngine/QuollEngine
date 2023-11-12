#pragma once

#include "quoll/lua-scripting/LuaUserTypeBase.h"

namespace quoll {

/**
 * @brief Lua scripting interface for script
 */
class ScriptLuaTable {
public:
  /**
   * @brief Create script table
   *
   * @param entity Entity
   * @param scriptGlobals Script globals
   */
  ScriptLuaTable(Entity entity, ScriptGlobals scriptGlobals);

  /**
   * @brief Get script variable
   *
   * @param name Variable name
   * @return Script data value
   */
  sol::object get(const String &name);

  /**
   * @brief Set script variable
   *
   * @param name Variable name
   * @param value Variable value
   */
  void set(const String &name, sol::object value);

  /**
   * @brief Get component name in scripts
   *
   * @return Component name
   */
  static const String getName() { return "script"; }

  /**
   * @brief Create user type
   *
   * @param usertype User type
   */
  static void create(sol::usertype<ScriptLuaTable> usertype);

private:
  Entity mEntity;
  ScriptGlobals mScriptGlobals;
};

} // namespace quoll
