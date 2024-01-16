#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "quoll/animation/AnimatorEvent.h"
#include "quoll/entity/EntityDatabase.h"
#include "quoll/lua-scripting/Messages.h"
#include "AnimatorLuaTable.h"

namespace quoll {

AnimatorLuaTable::AnimatorLuaTable(Entity entity, ScriptGlobals scriptGlobals)
    : mEntity(entity), mScriptGlobals(scriptGlobals) {}

sol_maybe<f32> AnimatorLuaTable::getNormalizedTime() {
  if (!mScriptGlobals.entityDatabase.has<Animator>(mEntity)) {
    Engine::getUserLogger().error()
        << lua::Messages::componentDoesNotExist(getName(), mEntity);
    return sol::nil;
  }

  return mScriptGlobals.entityDatabase.get<Animator>(mEntity).normalizedTime;
}

sol_maybe<AnimationStateLuaTable> AnimatorLuaTable::getCurrentState() {
  if (!mScriptGlobals.entityDatabase.has<Animator>(mEntity)) {
    Engine::getUserLogger().error()
        << lua::Messages::componentDoesNotExist(getName(), mEntity);
    return sol::nil;
  }

  const auto &animator = mScriptGlobals.entityDatabase.get<Animator>(mEntity);
  auto &state = mScriptGlobals.assetRegistry.getAnimators()
                    .getAsset(animator.asset)
                    .data.states.at(animator.currentState);

  return AnimationStateLuaTable(state);
}

void AnimatorLuaTable::trigger(String event) {
  mScriptGlobals.entityDatabase.set<AnimatorEvent>(mEntity, {event});
}

void AnimatorLuaTable::deleteThis() {
  if (mScriptGlobals.entityDatabase.has<Animator>(mEntity)) {
    mScriptGlobals.entityDatabase.remove<Animator>(mEntity);
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
