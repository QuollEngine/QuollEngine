#include "quoll/core/Base.h"

#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

class AudioLuaScriptingInterfaceTest : public LuaScriptingInterfaceTestBase {
public:
};

TEST_F(AudioLuaScriptingInterfaceTest,
       PlayDoesNothingIfAudioSourceComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  call(entity, "audio_play");

  EXPECT_FALSE(entityDatabase.has<quoll::AudioStart>(entity));
}

TEST_F(AudioLuaScriptingInterfaceTest,
       PlayAddsAudioStartComponentIfAudioSourceExists) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::AudioSource>(entity, {});
  call(entity, "audio_play");

  EXPECT_TRUE(entityDatabase.has<quoll::AudioStart>(entity));
}

TEST_F(AudioLuaScriptingInterfaceTest,
       IsPlayingReturnsFalseIfAudioStatusComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  auto state = call(entity, "audio_is_playing");

  EXPECT_FALSE(state["audio_is_playing_flag"].get<bool>());
}

TEST_F(AudioLuaScriptingInterfaceTest,
       IsPlayingReturnsTrueIfAudioStatusComponentExists) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::AudioStatus>(entity, {});
  auto state = call(entity, "audio_is_playing");

  EXPECT_TRUE(state["audio_is_playing_flag"].get<bool>());
}

TEST_F(AudioLuaScriptingInterfaceTest,
       DeleteDoesNothingIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();

  call(entity, "audio_delete");
  EXPECT_FALSE(entityDatabase.has<quoll::AudioSource>(entity));
}

TEST_F(AudioLuaScriptingInterfaceTest,
       DeleteRemovesAudioSourceComponentFromEntity) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::AudioSource>(entity, {});

  call(entity, "audio_delete");
  EXPECT_FALSE(entityDatabase.has<quoll::AudioSource>(entity));
}
