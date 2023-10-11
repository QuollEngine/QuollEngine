#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"

#include "quoll/entity/EntityDatabase.h"
#include "quoll/scripting/LuaMessages.h"

#include "AnimatorScriptingInterface.h"

namespace quoll {

AnimatorScriptingInterface::LuaInterface::LuaInterface(
    Entity entity, ScriptGlobals scriptGlobals)
    : mEntity(entity), mScriptGlobals(scriptGlobals) {}

void AnimatorScriptingInterface::LuaInterface::trigger(String event) {
  mScriptGlobals.entityDatabase.set<AnimatorEvent>(mEntity, {event});
}

void AnimatorScriptingInterface::LuaInterface::deleteThis() {
  if (mScriptGlobals.entityDatabase.has<Animator>(mEntity)) {
    mScriptGlobals.entityDatabase.remove<Animator>(mEntity);
  }
}

void AnimatorScriptingInterface::LuaInterface::create(
    sol::usertype<AnimatorScriptingInterface::LuaInterface> usertype) {
  usertype["trigger"] = &LuaInterface::trigger;
  usertype["delete"] = &LuaInterface::deleteThis;
}

} // namespace quoll
