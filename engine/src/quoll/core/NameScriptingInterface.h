#pragma once

#include "quoll/scripting/ComponentLuaInterface.h"

namespace quoll {

/**
 * @brief Scripting interfaces for name component
 */
struct NameScriptingInterface {
  class LuaInterface;
};

/**
 * @brief Lua interface for name component
 */
class NameScriptingInterface::LuaInterface {
public:
  /**
   * @brief Create name
   *
   * @param entity Entity
   * @param scriptGlobals Script globals
   */
  LuaInterface(Entity entity, ScriptGlobals scriptGlobals);

  /**
   * @brief Get name
   *
   * @return Name
   */
  String get();

  /**
   * @brief Set name
   *
   * @param name Name
   */
  void set(String name);

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
  create(sol::usertype<NameScriptingInterface::LuaInterface> usertype);

  /**
   * @brief Get component name in scripts
   *
   * @return Component name
   */
  static const String getName() { return "name"; }

private:
  Entity mEntity;
  ScriptGlobals mScriptGlobals;
};

} // namespace quoll
