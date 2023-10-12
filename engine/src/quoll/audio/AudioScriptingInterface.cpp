#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"

#include "quoll/entity/EntityDatabase.h"
#include "quoll/scripting/LuaMessages.h"

#include "AudioScriptingInterface.h"

namespace quoll {

AudioScriptingInterface::LuaInterface::LuaInterface(Entity entity,
                                                    ScriptGlobals scriptGlobals)
    : mEntity(entity), mScriptGlobals(scriptGlobals) {}

void AudioScriptingInterface::LuaInterface::play() {
  if (mScriptGlobals.entityDatabase.has<AudioSource>(mEntity)) {
    mScriptGlobals.entityDatabase.set<AudioStart>(mEntity, {});
  }
}

bool AudioScriptingInterface::LuaInterface::isPlaying() {
  return mScriptGlobals.entityDatabase.has<AudioStatus>(mEntity);
}

void AudioScriptingInterface::LuaInterface::deleteThis() {
  if (mScriptGlobals.entityDatabase.has<AudioSource>(mEntity)) {
    mScriptGlobals.entityDatabase.remove<AudioSource>(mEntity);
  }
}

void AudioScriptingInterface::LuaInterface::create(
    sol::usertype<AudioScriptingInterface::LuaInterface> usertype) {
  usertype["play"] = &LuaInterface::play;
  usertype["is_playing"] = &LuaInterface::isPlaying;
  usertype["delete"] = &LuaInterface::deleteThis;
}

} // namespace quoll
