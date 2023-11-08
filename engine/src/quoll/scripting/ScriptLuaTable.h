#pragma once

#include "quoll/scripting/ComponentLuaInterface.h"

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
