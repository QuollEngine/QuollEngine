#include "quoll/core/Base.h"
#include "quoll/audio/AudioSource.h"
#include "quoll/audio/AudioStart.h"
#include "quoll/audio/AudioStatus.h"
#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

class AudioLuaTableTest : public LuaScriptingInterfaceTestBase {
public:
};

TEST_F(AudioLuaTableTest, PlayDoesNothingIfAudioSourceComponentDoesNotExist) {
  auto entity = entityDatabase.entity();
  call(entity, "audioPlay");

  EXPECT_FALSE(entity.has<quoll::AudioStart>());
}

TEST_F(AudioLuaTableTest, PlayAddsAudioStartComponentIfAudioSourceExists) {
  auto entity = entityDatabase.entity();
  entity.set<quoll::AudioSource>({});
  call(entity, "audioPlay");

  EXPECT_TRUE(entity.has<quoll::AudioStart>());
}

TEST_F(AudioLuaTableTest,
       IsPlayingReturnsFalseIfAudioStatusComponentDoesNotExist) {
  auto entity = entityDatabase.entity();
  auto state = call(entity, "audioIsPlaying");

  EXPECT_FALSE(state["audioIsPlayingFlag"].get<bool>());
}

TEST_F(AudioLuaTableTest, IsPlayingReturnsTrueIfAudioStatusComponentExists) {
  auto entity = entityDatabase.entity();
  entity.set<quoll::AudioStatus>({});
  auto state = call(entity, "audioIsPlaying");

  EXPECT_TRUE(state["audioIsPlayingFlag"].get<bool>());
}

TEST_F(AudioLuaTableTest, DeleteDoesNothingIfComponentDoesNotExist) {
  auto entity = entityDatabase.entity();

  call(entity, "audioDelete");
  EXPECT_FALSE(entity.has<quoll::AudioSource>());
}

TEST_F(AudioLuaTableTest, DeleteRemovesAudioSourceComponentFromEntity) {
  auto entity = entityDatabase.entity();
  entity.set<quoll::AudioSource>({});

  call(entity, "audioDelete");
  EXPECT_FALSE(entity.has<quoll::AudioSource>());
}
