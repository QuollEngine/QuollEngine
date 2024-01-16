#pragma once

#include "quoll/asset/AnimatorAsset.h"
#include "quoll/lua-scripting/LuaUserTypeBase.h"

namespace quoll {

class AnimationStateLuaTable {
public:
  AnimationStateLuaTable(AnimationState &state);

  String getName();

  static void create(sol::state_view state);

private:
  AnimationState &mState;
};

} // namespace quoll
