#include "liquid/core/Base.h"
#include "liquid/asset/AssetManager.h"
#include "liquid/scripting/ScriptingSystem.h"

#include "liquid-tests/Testing.h"

class AudioLuaScriptingInterfaceTest : public ::testing::Test {
public:
  AudioLuaScriptingInterfaceTest()
      : assetManager(std::filesystem::current_path()),
        scriptingSystem(eventSystem, assetManager.getRegistry()) {}

  liquid::LuaScope &call(liquid::Entity entity,
                         const liquid::String &functionName) {
    auto handle =
        assetManager
            .loadLuaScriptFromFile(std::filesystem::current_path() /
                                   "scripting-system-component-tester.lua")
            .getData();

    entityDatabase.setComponent<liquid::ScriptingComponent>(entity, {handle});

    scriptingSystem.start(entityDatabase);

    auto &scripting =
        entityDatabase.getComponent<liquid::ScriptingComponent>(entity);

    scripting.scope.luaGetGlobal(functionName);
    scripting.scope.call(0);

    return scripting.scope;
  }

  liquid::EntityDatabase entityDatabase;
  liquid::EventSystem eventSystem;
  liquid::AssetManager assetManager;
  liquid::ScriptingSystem scriptingSystem;
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
