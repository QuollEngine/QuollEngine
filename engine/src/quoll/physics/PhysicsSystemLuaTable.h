#pragma once

#include "quoll/lua-scripting/ScriptGlobals.h"
#include "quoll/lua-scripting/ScriptSignal.h"
#include "quoll/lua-scripting/ScriptSignalView.h"

namespace quoll {

/**
 * @brief Physics system lua table
 */
class PhysicsSystemLuaTable {
public:
  /**
   * @brief Physics system lua table
   *
   * @param entity Entity
   * @param scriptGlobals Script globals
   */
  PhysicsSystemLuaTable(Entity entity, ScriptGlobals scriptGlobals);

  /**
   * @brief Get on collision start signal
   *
   * @return On collision start signal
   */
  lua::ScriptSignalView onCollisionStart();

  /**
   * @brief Get on collision end signal
   *
   * @return On collision end signal
   */
  lua::ScriptSignalView onCollisionEnd();

  /**
   * @brief Create physics system lua table
   *
   * @param state Sol state
   * @param entity Entity
   * @param scriptGlobals Script globals
   * @return Physics system lua table
   */
  static PhysicsSystemLuaTable create(sol::state_view state, Entity entity,
                                      ScriptGlobals scriptGlobals);

private:
  Entity mEntity;
  ScriptGlobals mScriptGlobals;
};

} // namespace quoll
