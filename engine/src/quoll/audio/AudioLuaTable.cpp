#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "quoll/audio/AudioSource.h"
#include "quoll/audio/AudioStart.h"
#include "quoll/audio/AudioStatus.h"

#include "quoll/entity/EntityDatabase.h"

#include "AudioLuaTable.h"

namespace quoll {

AudioLuaTable::AudioLuaTable(Entity entity, ScriptGlobals scriptGlobals)
    : mEntity(entity), mScriptGlobals(scriptGlobals) {}

void AudioLuaTable::play() {
  if (mScriptGlobals.entityDatabase.has<AudioSource>(mEntity)) {
    mScriptGlobals.entityDatabase.set<AudioStart>(mEntity, {});
  }
}

bool AudioLuaTable::isPlaying() {
  return mScriptGlobals.entityDatabase.has<AudioStatus>(mEntity);
}

void AudioLuaTable::deleteThis() {
  if (mScriptGlobals.entityDatabase.has<AudioSource>(mEntity)) {
    mScriptGlobals.entityDatabase.remove<AudioSource>(mEntity);
  }
}

void AudioLuaTable::create(sol::usertype<AudioLuaTable> usertype) {
  usertype["play"] = &AudioLuaTable::play;
  usertype["is_playing"] = &AudioLuaTable::isPlaying;
  usertype["delete"] = &AudioLuaTable::deleteThis;
}

} // namespace quoll
