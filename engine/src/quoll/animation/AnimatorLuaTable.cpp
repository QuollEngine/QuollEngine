#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"

#include "quoll/entity/EntityDatabase.h"
#include "quoll/scripting/LuaMessages.h"

#include "AnimatorLuaTable.h"

namespace quoll {

AnimatorLuaTable::AnimatorLuaTable(Entity entity, ScriptGlobals scriptGlobals)
    : mEntity(entity), mScriptGlobals(scriptGlobals) {}

void AnimatorLuaTable::trigger(String event) {
  mScriptGlobals.entityDatabase.set<AnimatorEvent>(mEntity, {event});
}

void AnimatorLuaTable::deleteThis() {
  if (mScriptGlobals.entityDatabase.has<Animator>(mEntity)) {
    mScriptGlobals.entityDatabase.remove<Animator>(mEntity);
  }
}

void AnimatorLuaTable::create(sol::usertype<AnimatorLuaTable> usertype) {
  usertype["trigger"] = &AnimatorLuaTable::trigger;
  usertype["delete"] = &AnimatorLuaTable::deleteThis;
}

} // namespace quoll
