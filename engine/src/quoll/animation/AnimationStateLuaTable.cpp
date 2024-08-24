#include "quoll/core/Base.h"
#include "AnimationStateLuaTable.h"
#include "AnimatorAsset.h"

namespace quoll {

AnimationStateLuaTable::AnimationStateLuaTable(AnimationState &state)
    : mState(state) {}

String AnimationStateLuaTable::getName() { return mState.name; }

void AnimationStateLuaTable::create(sol::state_view state) {
  auto usertype = state.new_usertype<AnimationStateLuaTable>(
      "AnimationState", sol::no_constructor);

  usertype["name"] = sol::property(&AnimationStateLuaTable::getName);
}

} // namespace quoll
