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
  if (mEntity.has<AudioSource>()) {
    mEntity.add<AudioStart>();
  }
}

bool AudioLuaTable::isPlaying() { return mEntity.has<AudioStatus>(); }

void AudioLuaTable::deleteThis() {
  if (mEntity.has<AudioSource>()) {
    mEntity.remove<AudioSource>();
  }
}

void AudioLuaTable::create(sol::usertype<AudioLuaTable> usertype,
                           sol::state_view state) {
  usertype["play"] = &AudioLuaTable::play;
  usertype["isPlaying"] = sol::property(&AudioLuaTable::isPlaying);
  usertype["delete"] = &AudioLuaTable::deleteThis;
}

} // namespace quoll
