#pragma once

#include "quoll/asset/AnimatorAsset.h"
#include "quoll/lua-scripting/LuaUserTypeBase.h"

namespace quoll {

/**
 * @brief Lua table for animation state
 */
class AnimationStateLuaTable {
public:
  /**
   * @brief Create animation state lua table
   *
   * @param state Animation state
   */
  AnimationStateLuaTable(AnimationState &state);

  /**
   * @brief Get name
   *
   * @return Name
   */
  String getName();

  /**
   * @brief Create animation state user type
   *
   * @param state Sol state
   */
  static void create(sol::state_view state);

private:
  AnimationState &mState;
};

} // namespace quoll
