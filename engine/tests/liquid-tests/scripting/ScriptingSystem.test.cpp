#include "liquid/core/Base.h"
#include "liquid/asset/AssetCache.h"
#include "liquid/scripting/ScriptingSystem.h"

#include "liquid-tests/Testing.h"

class ScriptingSystemTest : public ::testing::Test {
public:
  ScriptingSystemTest()
      : assetCache(FixturesPath),
        scriptingSystem(eventSystem, assetCache.getRegistry()) {
    scriptingSystem.observeChanges(entityDatabase);
  }

  liquid::EntityDatabase entityDatabase;
  liquid::EventSystem eventSystem;
  liquid::AssetCache assetCache;
  liquid::ScriptingSystem scriptingSystem;
};

using ScriptingSystemDeathTest = ScriptingSystemTest;

static constexpr float TimeDelta = 0.2f;

TEST_F(ScriptingSystemTest, CallsScriptingUpdateFunctionOnUpdate) {
  auto handle =
      assetCache
          .loadLuaScriptFromFile(FixturesPath / "scripting-system-tester.lua")
          .getData();

  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::Script>(entity, {handle});

  auto &component = entityDatabase.get<liquid::Script>(entity);
  EXPECT_EQ(component.scope.getLuaState(), nullptr);

  scriptingSystem.start(entityDatabase);
  EXPECT_NE(component.scope.getLuaState(), nullptr);

  scriptingSystem.update(TimeDelta, entityDatabase);
  EXPECT_EQ(component.scope.getGlobal<int32_t>("value"), 0);
  EXPECT_EQ(component.scope.getGlobal<float>("global_dt"), TimeDelta);
}

TEST_F(ScriptingSystemTest, DeletesScriptDataWhenComponentIsDeleted) {
  auto handle =
      assetCache
          .loadLuaScriptFromFile(FixturesPath / "scripting-system-tester.lua")
          .getData();

  static constexpr size_t NumEntities = 20;

  std::vector<liquid::Entity> entities(NumEntities, liquid::Entity::Null);
  for (size_t i = 0; i < entities.size(); ++i) {
    auto entity = entityDatabase.create();
    entities.at(i) = entity;

    entityDatabase.set<liquid::Script>(entity, {handle});
  }

  scriptingSystem.start(entityDatabase);

  std::vector<liquid::Script> scripts(entities.size());
  for (size_t i = 0; i < entities.size(); ++i) {
    auto entity = entities.at(i);
    scripts.at(i) = entityDatabase.get<liquid::Script>(entity);
    ASSERT_TRUE(eventSystem.hasObserver(liquid::CollisionEvent::CollisionEnded,
                                        scripts.at(i).onCollisionEnd));

    if ((i % 2) == 0) {
      entityDatabase.remove<liquid::Script>(entity);
    }
  }

  scriptingSystem.update(TimeDelta, entityDatabase);
  for (size_t i = 0; i < entities.size(); ++i) {
    auto entity = entities.at(i);
    bool deleted = (i % 2) == 0;
    EXPECT_NE(entityDatabase.has<liquid::Script>(entity), deleted);

    if (deleted) {
      EXPECT_FALSE(
          eventSystem.hasObserver(liquid::CollisionEvent::CollisionEnded,
                                  scripts.at(i).onCollisionEnd));
    }
  }
}

TEST_F(ScriptingSystemTest, CallsScriptingUpdateFunctionOnEveryUpdate) {
  auto handle =
      assetCache
          .loadLuaScriptFromFile(FixturesPath / "scripting-system-tester.lua")
          .getData();
  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::Script>(entity, {handle});

  auto &component = entityDatabase.get<liquid::Script>(entity);
  EXPECT_EQ(component.scope.getLuaState(), nullptr);

  scriptingSystem.start(entityDatabase);
  EXPECT_NE(component.scope.getLuaState(), nullptr);

  for (size_t i = 0; i < 10; ++i) {
    scriptingSystem.update(TimeDelta, entityDatabase);
  }

  EXPECT_EQ(component.scope.getGlobal<int32_t>("value"), 9);
}

TEST_F(ScriptingSystemTest, CallsScriptStartFunctionOnStart) {
  auto handle =
      assetCache
          .loadLuaScriptFromFile(FixturesPath / "scripting-system-tester.lua")
          .getData();
  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::Script>(entity, {handle});

  auto &component = entityDatabase.get<liquid::Script>(entity);
  EXPECT_EQ(component.scope.getLuaState(), nullptr);

  scriptingSystem.start(entityDatabase);
  EXPECT_NE(component.scope.getLuaState(), nullptr);

  auto *luaScope = component.scope.getLuaState();
  EXPECT_EQ(component.scope.getGlobal<int32_t>("value"), -1);
}

TEST_F(ScriptingSystemTest, CallsScriptingStartFunctionOnlyOnceOnStart) {
  auto handle =
      assetCache
          .loadLuaScriptFromFile(FixturesPath / "scripting-system-tester.lua")
          .getData();
  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::Script>(entity, {handle});

  auto &component = entityDatabase.get<liquid::Script>(entity);
  EXPECT_EQ(component.scope.getLuaState(), nullptr);

  // Call 10 times
  for (size_t i = 0; i < 10; ++i) {
    scriptingSystem.start(entityDatabase);
  }
  EXPECT_NE(component.scope.getLuaState(), nullptr);

  EXPECT_EQ(component.scope.getGlobal<int32_t>("value"), -1);
}

TEST_F(ScriptingSystemTest, RemovesScriptComponentIfInputVarsAreNotSet) {
  auto handle =
      assetCache
          .loadLuaScriptFromFile(FixturesPath / "scripting-system-vars.lua")
          .getData();

  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::Script>(entity, {handle});

  auto &component = entityDatabase.get<liquid::Script>(entity);

  scriptingSystem.start(entityDatabase);
  EXPECT_FALSE(entityDatabase.has<liquid::Script>(entity));
}

TEST_F(ScriptingSystemTest, RemovesScriptComponentIfInputVarTypesAreInvalid) {
  auto handle =
      assetCache
          .loadLuaScriptFromFile(FixturesPath / "scripting-system-vars.lua")
          .getData();

  auto entity = entityDatabase.create();
  liquid::Script script{handle};
  script.variables.insert_or_assign("string_value",
                                    liquid::PrefabAssetHandle{15});
  script.variables.insert_or_assign("prefab_value",
                                    liquid::PrefabAssetHandle{15});
  script.variables.insert_or_assign("texture_value",
                                    liquid::TextureAssetHandle{25});
  entityDatabase.set(entity, script);

  auto &component = entityDatabase.get<liquid::Script>(entity);

  scriptingSystem.start(entityDatabase);
  EXPECT_FALSE(entityDatabase.has<liquid::Script>(entity));
}

TEST_F(ScriptingSystemTest, SetsVariablesToInputVarsOnStart) {
  auto handle =
      assetCache
          .loadLuaScriptFromFile(FixturesPath / "scripting-system-vars.lua")
          .getData();

  auto entity = entityDatabase.create();
  liquid::Script script{handle};
  script.variables.insert_or_assign("string_value",
                                    liquid::String("Hello world"));
  script.variables.insert_or_assign("prefab_value",
                                    liquid::PrefabAssetHandle{15});
  script.variables.insert_or_assign("texture_value",
                                    liquid::TextureAssetHandle{25});
  entityDatabase.set(entity, script);

  auto &component = entityDatabase.get<liquid::Script>(entity);

  scriptingSystem.start(entityDatabase);
  ASSERT_TRUE(entityDatabase.has<liquid::Script>(entity));

  EXPECT_EQ(component.scope.getGlobal<liquid::String>("var_string"),
            "Hello world");
  EXPECT_EQ(component.scope.getGlobal<uint32_t>("var_prefab"), 15);
  EXPECT_EQ(component.scope.getGlobal<uint32_t>("var_texture"), 25);
}

TEST_F(ScriptingSystemTest, RemovesVariableSetterAfterInputVariablesAreSet) {
  auto handle =
      assetCache
          .loadLuaScriptFromFile(FixturesPath / "scripting-system-vars.lua")
          .getData();

  auto entity = entityDatabase.create();
  liquid::Script script{handle};
  script.variables.insert_or_assign("string_value",
                                    liquid::String("Hello world"));
  script.variables.insert_or_assign("prefab_value",
                                    liquid::PrefabAssetHandle{15});
  script.variables.insert_or_assign("texture_value",
                                    liquid::TextureAssetHandle{25});
  entityDatabase.set(entity, script);

  auto &component = entityDatabase.get<liquid::Script>(entity);

  scriptingSystem.start(entityDatabase);

  EXPECT_EQ(component.scope.getGlobal<liquid::String>("var_string"),
            "Hello world");
  EXPECT_EQ(component.scope.getGlobal<uint32_t>("var_prefab"), 15);
  EXPECT_EQ(component.scope.getGlobal<uint32_t>("var_texture"), 25);

  EXPECT_TRUE(component.scope.isGlobal<liquid::LuaTable>("global_vars"));
  EXPECT_TRUE(component.scope.isGlobal<liquid::LuaTable>("entity"));

  component.scope.luaGetGlobal("update");
  component.scope.call(0);

  EXPECT_TRUE(component.scope.isGlobal<std::nullptr_t>("global_vars"));
  EXPECT_TRUE(component.scope.isGlobal<liquid::LuaTable>("entity"));
}

TEST_F(ScriptingSystemTest, RegistersEventsOnStart) {
  auto handle =
      assetCache
          .loadLuaScriptFromFile(FixturesPath / "scripting-system-tester.lua")
          .getData();
  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::Script>(entity, {handle});

  auto &component = entityDatabase.get<liquid::Script>(entity);

  scriptingSystem.start(entityDatabase);

  EXPECT_LT(component.onCollisionStart, liquid::EventObserverMax);
  EXPECT_LT(component.onCollisionEnd, liquid::EventObserverMax);
  EXPECT_LT(component.onKeyPress, liquid::EventObserverMax);
  EXPECT_LT(component.onKeyRelease, liquid::EventObserverMax);
}

TEST_F(ScriptingSystemTest,
       DoesNotCallScriptCollisionEventIfEntityDidNotCollide) {
  auto handle =
      assetCache
          .loadLuaScriptFromFile(FixturesPath / "scripting-system-tester.lua")
          .getData();
  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::Script>(entity, {handle});

  auto &component = entityDatabase.get<liquid::Script>(entity);

  scriptingSystem.start(entityDatabase);

  eventSystem.dispatch(liquid::CollisionEvent::CollisionStarted,
                       {liquid::Entity{5}, liquid::Entity{6}});
  eventSystem.poll();

  EXPECT_EQ(component.scope.getGlobal<int32_t>("event"), 0);
}

TEST_F(ScriptingSystemTest, CallsScriptCollisionStartEventIfEntityCollided) {
  auto handle =
      assetCache
          .loadLuaScriptFromFile(FixturesPath / "scripting-system-tester.lua")
          .getData();
  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::Script>(entity, {handle});

  auto &component = entityDatabase.get<liquid::Script>(entity);

  scriptingSystem.start(entityDatabase);

  eventSystem.dispatch(liquid::CollisionEvent::CollisionStarted,
                       {entity, liquid::Entity{6}});
  eventSystem.poll();

  auto *luaScope = component.scope.getLuaState();
  EXPECT_EQ(component.scope.getGlobal<int32_t>("event"), 1);
  EXPECT_EQ(component.scope.getGlobal<int32_t>("target"), 6);
}

TEST_F(ScriptingSystemTest, CallsScriptCollisionEndEventIfEntityCollided) {
  auto handle =
      assetCache
          .loadLuaScriptFromFile(FixturesPath / "scripting-system-tester.lua")
          .getData();
  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::Script>(entity, {handle});

  auto &component = entityDatabase.get<liquid::Script>(entity);

  scriptingSystem.start(entityDatabase);

  eventSystem.dispatch(liquid::CollisionEvent::CollisionEnded,
                       {liquid::Entity{5}, entity});
  eventSystem.poll();

  EXPECT_EQ(component.scope.getGlobal<int32_t>("event"), 2);
  EXPECT_EQ(component.scope.getGlobal<int32_t>("target"), 5);
}

TEST_F(ScriptingSystemTest, CallsScriptKeyPressEventIfKeyIsPressed) {
  auto handle =
      assetCache
          .loadLuaScriptFromFile(FixturesPath / "scripting-system-tester.lua")
          .getData();
  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::Script>(entity, {handle});

  auto &component = entityDatabase.get<liquid::Script>(entity);

  scriptingSystem.start(entityDatabase);

  eventSystem.dispatch(liquid::KeyboardEvent::Pressed, {15});
  eventSystem.poll();

  auto *luaScope = component.scope.getLuaState();
  EXPECT_EQ(component.scope.getGlobal<int32_t>("event"), 3);
  EXPECT_EQ(component.scope.getGlobal<int32_t>("key_value"), 15);
}

TEST_F(ScriptingSystemTest, CallsScriptKeyReleaseEventIfKeyIsReleased) {
  auto handle =
      assetCache
          .loadLuaScriptFromFile(FixturesPath / "scripting-system-tester.lua")
          .getData();
  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::Script>(entity, {handle});

  auto &component = entityDatabase.get<liquid::Script>(entity);

  scriptingSystem.start(entityDatabase);

  eventSystem.dispatch(liquid::KeyboardEvent::Released, {35});
  eventSystem.poll();

  EXPECT_EQ(component.scope.getGlobal<int32_t>("event"), 4);
  EXPECT_EQ(component.scope.getGlobal<int32_t>("key_value"), 35);
}
