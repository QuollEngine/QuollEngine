#include "liquid/core/Base.h"

#include "liquid-tests/Testing.h"
#include "liquid-tests/test-utils/ScriptingInterfaceTestBase.h"

class AudioLuaScriptingInterfaceTest : public LuaScriptingInterfaceTestBase {
public:
};

TEST_F(AudioLuaScriptingInterfaceTest,
       PlayDoesNothingIfAudioSourceComponentDoesNotExist) {
  auto entity = entityDatabase.createEntity();
  call(entity, "audio_play");

  EXPECT_FALSE(
      entityDatabase.hasComponent<liquid::AudioStartComponent>(entity));
}

TEST_F(AudioLuaScriptingInterfaceTest, PlayDoesNothingIfInvalidArguments) {
  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::AudioSourceComponent>(entity, {});
  call(entity, "audio_play_invalid");

  EXPECT_FALSE(
      entityDatabase.hasComponent<liquid::AudioStartComponent>(entity));
}

TEST_F(AudioLuaScriptingInterfaceTest,
       PlayAddsAudioStartComponentIfAudioSourceExists) {
  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::AudioSourceComponent>(entity, {});
  call(entity, "audio_play");

  EXPECT_TRUE(entityDatabase.hasComponent<liquid::AudioStartComponent>(entity));
}

TEST_F(AudioLuaScriptingInterfaceTest,
       IsPlayingReturnsFalseIfAudioStatusComponentDoesNotExist) {
  auto entity = entityDatabase.createEntity();
  auto &scope = call(entity, "audio_is_playing");

  EXPECT_FALSE(scope.getGlobal<bool>("audio_is_playing_flag"));
}

TEST_F(AudioLuaScriptingInterfaceTest,
       IsPlayingReturnsFalseIfInvalidArguments) {
  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::AudioStatusComponent>(entity, {});

  auto &scope = call(entity, "audio_is_playing_invalid");
  EXPECT_FALSE(scope.getGlobal<bool>("audio_is_playing_flag"));
}

TEST_F(AudioLuaScriptingInterfaceTest,
       IsPlayingReturnsTrueIfAudioStatusComponentExists) {
  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::AudioStatusComponent>(entity, {});
  auto &scope = call(entity, "audio_is_playing");

  EXPECT_TRUE(scope.getGlobal<bool>("audio_is_playing_flag"));
}
