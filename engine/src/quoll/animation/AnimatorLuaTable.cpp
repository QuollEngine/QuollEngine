#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "quoll/animation/AnimatorEvent.h"
#include "quoll/asset/AssetRegistry.h"
#include "quoll/entity/EntityDatabase.h"
#include "quoll/lua-scripting/Messages.h"
#include "Animator.h"
#include "AnimatorLuaTable.h"

namespace quoll {

AnimatorLuaTable::AnimatorLuaTable(Entity entity, ScriptGlobals scriptGlobals)
    : mEntity(entity), mScriptGlobals(scriptGlobals) {}

sol_maybe<f32> AnimatorLuaTable::getNormalizedTime() {
  if (!mEntity.has<Animator>()) {
    Engine::getUserLogger().error()
        << lua::Messages::componentDoesNotExist(getName(), mEntity);
    return sol::nil;
  }

  return mEntity.get_ref<Animator>()->normalizedTime;
}

sol_maybe<AnimationStateLuaTable> AnimatorLuaTable::getCurrentState() {
  if (!mEntity.has<Animator>()) {
    Engine::getUserLogger().error()
        << lua::Messages::componentDoesNotExist(getName(), mEntity);
    return sol::nil;
  }

  auto animator = mEntity.get_ref<Animator>();
  auto &state = mScriptGlobals.assetRegistry.getAnimators()
                    .getAsset(animator->asset)
                    .data.states.at(animator->currentState);

  return AnimationStateLuaTable(state);
}

void AnimatorLuaTable::trigger(String event) {
  mEntity.set<AnimatorEvent>({event});
}

void AnimatorLuaTable::deleteThis() {
  if (mEntity.has<Animator>()) {
    mEntity.remove<Animator>();
  }
}

void AnimatorLuaTable::create(sol::usertype<AnimatorLuaTable> usertype,
                              sol::state_view state) {
  AnimationStateLuaTable::create(state);

  usertype["normalizedTime"] =
      sol::property(&AnimatorLuaTable::getNormalizedTime);
  usertype["currentState"] = sol::property(&AnimatorLuaTable::getCurrentState);
  usertype["trigger"] = &AnimatorLuaTable::trigger;
  usertype["delete"] = &AnimatorLuaTable::deleteThis;
}

} // namespace quoll
