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

TEST_F(AudioLuaScriptingInterfaceTest, PlayDoesNothingIfInvalidArguments) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::AudioSource>(entity, {});
  call(entity, "audio_play_invalid");

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
  auto &scope = call(entity, "audio_is_playing");

  EXPECT_FALSE(scope.getGlobal<bool>("audio_is_playing_flag"));
}

TEST_F(AudioLuaScriptingInterfaceTest,
       IsPlayingReturnsFalseIfInvalidArguments) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::AudioStatus>(entity, {});
  call(entity, "audio_is_playing_invalid");
}

TEST_F(AudioLuaScriptingInterfaceTest,
       IsPlayingReturnsTrueIfAudioStatusComponentExists) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::AudioStatus>(entity, {});
  auto &scope = call(entity, "audio_is_playing");

  EXPECT_TRUE(scope.getGlobal<bool>("audio_is_playing_flag"));
}

TEST_F(AudioLuaScriptingInterfaceTest,
       DeleteDoesNothingIfProvidedArgumentIsInvalid) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::AudioSource>(entity, {});

  call(entity, "audio_delete_invalid");
  EXPECT_TRUE(entityDatabase.has<quoll::AudioSource>(entity));
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
