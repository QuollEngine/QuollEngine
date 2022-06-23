#include "liquid/core/Base.h"
#include "liquid/asset/AssetManager.h"
#include "liquid/scripting/ScriptingSystem.h"

#include "liquid-tests/Testing.h"

class ScriptingSystemTest : public ::testing::Test {
public:
  ScriptingSystemTest()
      : assetManager(std::filesystem::current_path()),
        scriptingSystem(eventSystem, assetManager.getRegistry()) {}

  liquid::EntityDatabase entityDatabase;
  liquid::EventSystem eventSystem;
  liquid::AssetManager assetManager;
  liquid::ScriptingSystem scriptingSystem;
};

using ScriptingSystemDeathTest = ScriptingSystemTest;

constexpr float DELTA_TIME = 0.2f;

TEST_F(ScriptingSystemTest, CallsScriptingUpdateFunctionOnUpdate) {
  auto handle = assetManager
                    .loadLuaScriptFromFile(std::filesystem::current_path() /
                                           "scripting-system-tester.lua")
                    .getData();

  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::ScriptingComponent>(entity, {handle});

  auto &component =
      entityDatabase.getComponent<liquid::ScriptingComponent>(entity);
  EXPECT_EQ(component.scope.getLuaState(), nullptr);

  scriptingSystem.start(entityDatabase);
  EXPECT_NE(component.scope.getLuaState(), nullptr);

  scriptingSystem.update(DELTA_TIME, entityDatabase);
  EXPECT_EQ(component.scope.getGlobal<int32_t>("value"), 0);
  EXPECT_EQ(component.scope.getGlobal<float>("global_dt"), DELTA_TIME);
}

TEST_F(ScriptingSystemTest,
       DeleteScriptingComponentForEntitiesWithDeleteComponents) {
  auto handle = assetManager
                    .loadLuaScriptFromFile(std::filesystem::current_path() /
                                           "scripting-system-tester.lua")
                    .getData();

  static constexpr size_t NUM_ENTITIES = 20;

  std::vector<liquid::Entity> entities(NUM_ENTITIES, liquid::EntityNull);
  for (size_t i = 0; i < entities.size(); ++i) {
    auto entity = entityDatabase.createEntity();
    entities.at(i) = entity;

    entityDatabase.setComponent<liquid::ScriptingComponent>(entity, {handle});
    if ((i % 2) == 0) {
      entityDatabase.setComponent<liquid::DeleteComponent>(entity, {});
    }
  }

  scriptingSystem.start(entityDatabase);
  scriptingSystem.update(DELTA_TIME, entityDatabase);

  for (size_t i = 0; i < entities.size(); ++i) {
    auto entity = entities.at(i);
    EXPECT_NE(entityDatabase.hasComponent<liquid::ScriptingComponent>(entity),
              (i % 2) == 0);
  }
}

TEST_F(ScriptingSystemTest, CallsScriptingUpdateFunctionOnEveryUpdate) {
  auto handle = assetManager
                    .loadLuaScriptFromFile(std::filesystem::current_path() /
                                           "scripting-system-tester.lua")
                    .getData();
  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::ScriptingComponent>(entity, {handle});

  auto &component =
      entityDatabase.getComponent<liquid::ScriptingComponent>(entity);
  EXPECT_EQ(component.scope.getLuaState(), nullptr);

  scriptingSystem.start(entityDatabase);
  EXPECT_NE(component.scope.getLuaState(), nullptr);

  for (size_t i = 0; i < 10; ++i) {
    scriptingSystem.update(DELTA_TIME, entityDatabase);
  }

  EXPECT_EQ(component.scope.getGlobal<int32_t>("value"), 9);
}

TEST_F(ScriptingSystemTest, CallsScriptStartFunctionOnStart) {
  auto handle = assetManager
                    .loadLuaScriptFromFile(std::filesystem::current_path() /
                                           "scripting-system-tester.lua")
                    .getData();
  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::ScriptingComponent>(entity, {handle});

  auto &component =
      entityDatabase.getComponent<liquid::ScriptingComponent>(entity);
  EXPECT_EQ(component.scope.getLuaState(), nullptr);

  scriptingSystem.start(entityDatabase);
  EXPECT_NE(component.scope.getLuaState(), nullptr);

  auto *luaScope = component.scope.getLuaState();
  EXPECT_EQ(component.scope.getGlobal<int32_t>("value"), -1);
}

TEST_F(ScriptingSystemTest, CallsScriptingStartFunctionOnlyOnceOnStart) {
  auto handle = assetManager
                    .loadLuaScriptFromFile(std::filesystem::current_path() /
                                           "scripting-system-tester.lua")
                    .getData();
  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::ScriptingComponent>(entity, {handle});

  auto &component =
      entityDatabase.getComponent<liquid::ScriptingComponent>(entity);
  EXPECT_EQ(component.scope.getLuaState(), nullptr);

  // Call 10 times
  for (size_t i = 0; i < 10; ++i) {
    scriptingSystem.start(entityDatabase);
  }
  EXPECT_NE(component.scope.getLuaState(), nullptr);

  EXPECT_EQ(component.scope.getGlobal<int32_t>("value"), -1);
}

TEST_F(ScriptingSystemTest, RegistersEventsOnStart) {
  auto handle = assetManager
                    .loadLuaScriptFromFile(std::filesystem::current_path() /
                                           "scripting-system-tester.lua")
                    .getData();
  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::ScriptingComponent>(entity, {handle});

  auto &component =
      entityDatabase.getComponent<liquid::ScriptingComponent>(entity);

  scriptingSystem.start(entityDatabase);

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
  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::ScriptingComponent>(entity, {handle});

  auto &component =
      entityDatabase.getComponent<liquid::ScriptingComponent>(entity);

  scriptingSystem.start(entityDatabase);

  eventSystem.dispatch(liquid::CollisionEvent::CollisionStarted, {5, 6});
  eventSystem.poll();

  EXPECT_EQ(component.scope.getGlobal<int32_t>("event"), 0);
}

TEST_F(ScriptingSystemTest, CallsScriptCollisionStartEventIfEntityCollided) {
  auto handle = assetManager
                    .loadLuaScriptFromFile(std::filesystem::current_path() /
                                           "scripting-system-tester.lua")
                    .getData();
  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::ScriptingComponent>(entity, {handle});

  auto &component =
      entityDatabase.getComponent<liquid::ScriptingComponent>(entity);

  scriptingSystem.start(entityDatabase);

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
  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::ScriptingComponent>(entity, {handle});

  auto &component =
      entityDatabase.getComponent<liquid::ScriptingComponent>(entity);

  scriptingSystem.start(entityDatabase);

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
  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::ScriptingComponent>(entity, {handle});

  auto &component =
      entityDatabase.getComponent<liquid::ScriptingComponent>(entity);

  scriptingSystem.start(entityDatabase);

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
  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::ScriptingComponent>(entity, {handle});

  auto &component =
      entityDatabase.getComponent<liquid::ScriptingComponent>(entity);

  scriptingSystem.start(entityDatabase);

  eventSystem.dispatch(liquid::KeyboardEvent::Released, {35});
  eventSystem.poll();

  EXPECT_EQ(component.scope.getGlobal<int32_t>("event"), 4);
  EXPECT_EQ(component.scope.getGlobal<int32_t>("key_value"), 35);
}
