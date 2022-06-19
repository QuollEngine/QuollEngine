#include "liquid/core/Base.h"
#include "liquid/asset/AssetManager.h"
#include "liquid/scripting/ScriptingSystem.h"

#include "liquid-tests/Testing.h"

class ScriptingSystemTest : public ::testing::Test {
public:
  ScriptingSystemTest()
      : assetManager(std::filesystem::current_path()),
        scriptingSystem(eventSystem, assetManager.getRegistry()) {}

  liquid::EntityContext entityContext;
  liquid::EventSystem eventSystem;
  liquid::AssetManager assetManager;
  liquid::ScriptingSystem scriptingSystem;
};

using ScriptingSystemDeathTest = ScriptingSystemTest;

TEST_F(ScriptingSystemTest, CallsScriptingUpdateFunctionOnUpdate) {
  auto handle = assetManager
                    .loadLuaScriptFromFile(std::filesystem::current_path() /
                                           "scripting-system-tester.lua")
                    .getData();

  auto entity = entityContext.createEntity();
  entityContext.setComponent<liquid::ScriptingComponent>(entity, {handle});

  auto &component =
      entityContext.getComponent<liquid::ScriptingComponent>(entity);
  EXPECT_EQ(component.scope.getLuaState(), nullptr);

  scriptingSystem.start(entityContext);
  EXPECT_NE(component.scope.getLuaState(), nullptr);

  scriptingSystem.update(entityContext);
  EXPECT_EQ(component.scope.getGlobal<int32_t>("value"), 0);
}

TEST_F(ScriptingSystemTest, CallsScriptingUpdateFunctionOnEveryUpdate) {
  auto handle = assetManager
                    .loadLuaScriptFromFile(std::filesystem::current_path() /
                                           "scripting-system-tester.lua")
                    .getData();
  auto entity = entityContext.createEntity();
  entityContext.setComponent<liquid::ScriptingComponent>(entity, {handle});

  auto &component =
      entityContext.getComponent<liquid::ScriptingComponent>(entity);
  EXPECT_EQ(component.scope.getLuaState(), nullptr);

  scriptingSystem.start(entityContext);
  EXPECT_NE(component.scope.getLuaState(), nullptr);

  for (size_t i = 0; i < 10; ++i) {
    scriptingSystem.update(entityContext);
  }

  EXPECT_EQ(component.scope.getGlobal<int32_t>("value"), 9);
}

TEST_F(ScriptingSystemTest, CallsScriptStartFunctionOnStart) {
  auto handle = assetManager
                    .loadLuaScriptFromFile(std::filesystem::current_path() /
                                           "scripting-system-tester.lua")
                    .getData();
  auto entity = entityContext.createEntity();
  entityContext.setComponent<liquid::ScriptingComponent>(entity, {handle});

  auto &component =
      entityContext.getComponent<liquid::ScriptingComponent>(entity);
  EXPECT_EQ(component.scope.getLuaState(), nullptr);

  scriptingSystem.start(entityContext);
  EXPECT_NE(component.scope.getLuaState(), nullptr);

  auto *luaScope = component.scope.getLuaState();
  EXPECT_EQ(component.scope.getGlobal<int32_t>("value"), -1);
}

TEST_F(ScriptingSystemTest, CallsScriptingStartFunctionOnlyOnceOnStart) {
  auto handle = assetManager
                    .loadLuaScriptFromFile(std::filesystem::current_path() /
                                           "scripting-system-tester.lua")
                    .getData();
  auto entity = entityContext.createEntity();
  entityContext.setComponent<liquid::ScriptingComponent>(entity, {handle});

  auto &component =
      entityContext.getComponent<liquid::ScriptingComponent>(entity);
  EXPECT_EQ(component.scope.getLuaState(), nullptr);

  // Call 10 times
  for (size_t i = 0; i < 10; ++i) {
    scriptingSystem.start(entityContext);
  }
  EXPECT_NE(component.scope.getLuaState(), nullptr);

  EXPECT_EQ(component.scope.getGlobal<int32_t>("value"), -1);
}

TEST_F(ScriptingSystemTest, RegistersEventsOnStart) {
  auto handle = assetManager
                    .loadLuaScriptFromFile(std::filesystem::current_path() /
                                           "scripting-system-tester.lua")
                    .getData();
  auto entity = entityContext.createEntity();
  entityContext.setComponent<liquid::ScriptingComponent>(entity, {handle});

  auto &component =
      entityContext.getComponent<liquid::ScriptingComponent>(entity);

  scriptingSystem.start(entityContext);

  EXPECT_LT(component.onCollisionStart, liquid::EVENT_OBSERVER_MAX);
  EXPECT_LT(component.onCollisionEnd, liquid::EVENT_OBSERVER_MAX);
  EXPECT_LT(component.onKeyPress, liquid::EVENT_OBSERVER_MAX);
  EXPECT_LT(component.onKeyRelease, liquid::EVENT_OBSERVER_MAX);
}

TEST_F(ScriptingSystemTest,
       DoesNotCallScriptCollisionEventIfEntityDidNotCollide) {
  auto handle = assetManager
                    .loadLuaScriptFromFile(std::filesystem::current_path() /
                                           "scripting-system-tester.lua")
                    .getData();
  auto entity = entityContext.createEntity();
  entityContext.setComponent<liquid::ScriptingComponent>(entity, {handle});

  auto &component =
      entityContext.getComponent<liquid::ScriptingComponent>(entity);

  scriptingSystem.start(entityContext);

  eventSystem.dispatch(liquid::CollisionEvent::CollisionStarted, {5, 6});
  eventSystem.poll();

  EXPECT_EQ(component.scope.getGlobal<int32_t>("event"), 0);
}

TEST_F(ScriptingSystemTest, CallsScriptCollisionStartEventIfEntityCollided) {
  auto handle = assetManager
                    .loadLuaScriptFromFile(std::filesystem::current_path() /
                                           "scripting-system-tester.lua")
                    .getData();
  auto entity = entityContext.createEntity();
  entityContext.setComponent<liquid::ScriptingComponent>(entity, {handle});

  auto &component =
      entityContext.getComponent<liquid::ScriptingComponent>(entity);

  scriptingSystem.start(entityContext);

  eventSystem.dispatch(liquid::CollisionEvent::CollisionStarted, {entity, 6});
  eventSystem.poll();

  auto *luaScope = component.scope.getLuaState();
  EXPECT_EQ(component.scope.getGlobal<int32_t>("event"), 1);
  EXPECT_EQ(component.scope.getGlobal<int32_t>("target"), 6);
}

TEST_F(ScriptingSystemTest, CallsScriptCollisionEndEventIfEntityCollided) {
  auto handle = assetManager
                    .loadLuaScriptFromFile(std::filesystem::current_path() /
                                           "scripting-system-tester.lua")
                    .getData();
  auto entity = entityContext.createEntity();
  entityContext.setComponent<liquid::ScriptingComponent>(entity, {handle});

  auto &component =
      entityContext.getComponent<liquid::ScriptingComponent>(entity);

  scriptingSystem.start(entityContext);

  eventSystem.dispatch(liquid::CollisionEvent::CollisionEnded, {5, entity});
  eventSystem.poll();

  EXPECT_EQ(component.scope.getGlobal<int32_t>("event"), 2);
  EXPECT_EQ(component.scope.getGlobal<int32_t>("target"), 5);
}

TEST_F(ScriptingSystemTest, CallsScriptKeyPressEventIfKeyIsPressed) {
  auto handle = assetManager
                    .loadLuaScriptFromFile(std::filesystem::current_path() /
                                           "scripting-system-tester.lua")
                    .getData();
  auto entity = entityContext.createEntity();
  entityContext.setComponent<liquid::ScriptingComponent>(entity, {handle});

  auto &component =
      entityContext.getComponent<liquid::ScriptingComponent>(entity);

  scriptingSystem.start(entityContext);

  eventSystem.dispatch(liquid::KeyboardEvent::Pressed, {15});
  eventSystem.poll();

  auto *luaScope = component.scope.getLuaState();
  EXPECT_EQ(component.scope.getGlobal<int32_t>("event"), 3);
  EXPECT_EQ(component.scope.getGlobal<int32_t>("key_value"), 15);
}

TEST_F(ScriptingSystemTest, CallsScriptKeyReleaseEventIfKeyIsReleased) {
  auto handle = assetManager
                    .loadLuaScriptFromFile(std::filesystem::current_path() /
                                           "scripting-system-tester.lua")
                    .getData();
  auto entity = entityContext.createEntity();
  entityContext.setComponent<liquid::ScriptingComponent>(entity, {handle});

  auto &component =
      entityContext.getComponent<liquid::ScriptingComponent>(entity);

  scriptingSystem.start(entityContext);

  eventSystem.dispatch(liquid::KeyboardEvent::Released, {35});
  eventSystem.poll();

  EXPECT_EQ(component.scope.getGlobal<int32_t>("event"), 4);
  EXPECT_EQ(component.scope.getGlobal<int32_t>("key_value"), 35);
}
