#include "quoll/core/Base.h"
#include "quoll/asset/AssetCache.h"
#include "quoll/scripting/ScriptingSystem.h"

#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/AssetCacheTestBase.h"

class ScriptingSystemTest : public AssetCacheTestBase {
public:
  ScriptingSystemTest() : scriptingSystem(eventSystem, cache.getRegistry()) {
    scriptingSystem.observeChanges(entityDatabase);
  }

  quoll::LuaScriptAssetHandle
  loadLuaScript(quoll::String filename = "scripting-system-tester.lua") {
    auto uuid = quoll::Uuid::generate();
    cache.createLuaScriptFromSource(FixturesPath / filename, uuid);
    return cache.loadLuaScript(uuid).getData();
  }

  quoll::EntityDatabase entityDatabase;
  quoll::EventSystem eventSystem;
  quoll::ScriptingSystem scriptingSystem;
};

using ScriptingSystemDeathTest = ScriptingSystemTest;

static constexpr float TimeDelta = 0.2f;

TEST_F(ScriptingSystemTest, CallsScriptingUpdateFunctionOnUpdate) {
  auto handle = loadLuaScript();

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Script>(entity, {handle});

  auto &component = entityDatabase.get<quoll::Script>(entity);
  EXPECT_EQ(component.scope.getLuaState(), nullptr);

  scriptingSystem.start(entityDatabase);
  EXPECT_NE(component.scope.getLuaState(), nullptr);

  scriptingSystem.update(TimeDelta, entityDatabase);
  EXPECT_EQ(component.scope.getGlobal<int32_t>("value"), 0);
  EXPECT_EQ(component.scope.getGlobal<float>("global_dt"), TimeDelta);
}

TEST_F(ScriptingSystemTest, DeletesScriptDataWhenComponentIsDeleted) {
  auto handle = loadLuaScript();

  static constexpr size_t NumEntities = 20;

  std::vector<quoll::Entity> entities(NumEntities, quoll::Entity::Null);
  for (size_t i = 0; i < entities.size(); ++i) {
    auto entity = entityDatabase.create();
    entities.at(i) = entity;

    entityDatabase.set<quoll::Script>(entity, {handle});
  }

  scriptingSystem.start(entityDatabase);

  std::vector<quoll::Script> scripts(entities.size());
  for (size_t i = 0; i < entities.size(); ++i) {
    auto entity = entities.at(i);
    scripts.at(i) = entityDatabase.get<quoll::Script>(entity);
    ASSERT_TRUE(eventSystem.hasObserver(quoll::CollisionEvent::CollisionEnded,
                                        scripts.at(i).onCollisionEnd));

    if ((i % 2) == 0) {
      entityDatabase.remove<quoll::Script>(entity);
    }
  }

  scriptingSystem.update(TimeDelta, entityDatabase);
  for (size_t i = 0; i < entities.size(); ++i) {
    auto entity = entities.at(i);
    bool deleted = (i % 2) == 0;
    EXPECT_NE(entityDatabase.has<quoll::Script>(entity), deleted);

    if (deleted) {
      EXPECT_FALSE(eventSystem.hasObserver(
          quoll::CollisionEvent::CollisionEnded, scripts.at(i).onCollisionEnd));
    }
  }
}

TEST_F(ScriptingSystemTest, CallsScriptingUpdateFunctionOnEveryUpdate) {
  auto handle = loadLuaScript();

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Script>(entity, {handle});

  auto &component = entityDatabase.get<quoll::Script>(entity);
  EXPECT_EQ(component.scope.getLuaState(), nullptr);

  scriptingSystem.start(entityDatabase);
  EXPECT_NE(component.scope.getLuaState(), nullptr);

  for (size_t i = 0; i < 10; ++i) {
    scriptingSystem.update(TimeDelta, entityDatabase);
  }

  EXPECT_EQ(component.scope.getGlobal<int32_t>("value"), 9);
}

TEST_F(ScriptingSystemTest, CallsScriptStartFunctionOnStart) {
  auto handle = loadLuaScript();

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Script>(entity, {handle});

  auto &component = entityDatabase.get<quoll::Script>(entity);
  EXPECT_EQ(component.scope.getLuaState(), nullptr);

  scriptingSystem.start(entityDatabase);
  EXPECT_NE(component.scope.getLuaState(), nullptr);

  auto *luaScope = component.scope.getLuaState();
  EXPECT_EQ(component.scope.getGlobal<int32_t>("value"), -1);
}

TEST_F(ScriptingSystemTest, CallsScriptingStartFunctionOnlyOnceOnStart) {
  auto handle = loadLuaScript();

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Script>(entity, {handle});

  auto &component = entityDatabase.get<quoll::Script>(entity);
  EXPECT_EQ(component.scope.getLuaState(), nullptr);

  // Call 10 times
  for (size_t i = 0; i < 10; ++i) {
    scriptingSystem.start(entityDatabase);
  }
  EXPECT_NE(component.scope.getLuaState(), nullptr);

  EXPECT_EQ(component.scope.getGlobal<int32_t>("value"), -1);
}

TEST_F(ScriptingSystemTest, RemovesScriptComponentIfInputVarsAreNotSet) {
  auto handle = loadLuaScript("scripting-system-vars.lua");

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Script>(entity, {handle});

  auto &component = entityDatabase.get<quoll::Script>(entity);

  scriptingSystem.start(entityDatabase);
  EXPECT_FALSE(entityDatabase.has<quoll::Script>(entity));
}

TEST_F(ScriptingSystemTest, RemovesScriptComponentIfInputVarTypesAreInvalid) {
  auto handle = loadLuaScript("scripting-system-vars.lua");

  auto entity = entityDatabase.create();
  quoll::Script script{handle};
  script.variables.insert_or_assign("string_value",
                                    quoll::PrefabAssetHandle{15});
  script.variables.insert_or_assign("prefab_value",
                                    quoll::PrefabAssetHandle{15});
  script.variables.insert_or_assign("texture_value",
                                    quoll::TextureAssetHandle{25});
  entityDatabase.set(entity, script);

  auto &component = entityDatabase.get<quoll::Script>(entity);

  scriptingSystem.start(entityDatabase);
  EXPECT_FALSE(entityDatabase.has<quoll::Script>(entity));
}

TEST_F(ScriptingSystemTest, SetsVariablesToInputVarsOnStart) {
  auto handle = loadLuaScript("scripting-system-vars.lua");

  auto entity = entityDatabase.create();
  quoll::Script script{handle};
  script.variables.insert_or_assign("string_value",
                                    quoll::String("Hello world"));
  script.variables.insert_or_assign("prefab_value",
                                    quoll::PrefabAssetHandle{15});
  script.variables.insert_or_assign("texture_value",
                                    quoll::TextureAssetHandle{25});
  entityDatabase.set(entity, script);

  auto &component = entityDatabase.get<quoll::Script>(entity);

  scriptingSystem.start(entityDatabase);
  ASSERT_TRUE(entityDatabase.has<quoll::Script>(entity));

  EXPECT_EQ(component.scope.getGlobal<quoll::String>("var_string"),
            "Hello world");
  EXPECT_EQ(component.scope.getGlobal<uint32_t>("var_prefab"), 15);
  EXPECT_EQ(component.scope.getGlobal<uint32_t>("var_texture"), 25);
}

TEST_F(ScriptingSystemTest, RemovesVariableSetterAfterInputVariablesAreSet) {
  auto handle = loadLuaScript("scripting-system-vars.lua");

  auto entity = entityDatabase.create();
  quoll::Script script{handle};
  script.variables.insert_or_assign("string_value",
                                    quoll::String("Hello world"));
  script.variables.insert_or_assign("prefab_value",
                                    quoll::PrefabAssetHandle{15});
  script.variables.insert_or_assign("texture_value",
                                    quoll::TextureAssetHandle{25});
  entityDatabase.set(entity, script);

  auto &component = entityDatabase.get<quoll::Script>(entity);

  scriptingSystem.start(entityDatabase);

  EXPECT_EQ(component.scope.getGlobal<quoll::String>("var_string"),
            "Hello world");
  EXPECT_EQ(component.scope.getGlobal<uint32_t>("var_prefab"), 15);
  EXPECT_EQ(component.scope.getGlobal<uint32_t>("var_texture"), 25);

  EXPECT_TRUE(component.scope.isGlobal<quoll::LuaTable>("global_vars"));
  EXPECT_TRUE(component.scope.isGlobal<quoll::LuaTable>("entity"));

  component.scope.luaGetGlobal("update");
  component.scope.call(0);

  EXPECT_TRUE(component.scope.isGlobal<std::nullptr_t>("global_vars"));
  EXPECT_TRUE(component.scope.isGlobal<quoll::LuaTable>("entity"));
}

TEST_F(ScriptingSystemTest, RegistersEventsOnStart) {
  auto handle = loadLuaScript();

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Script>(entity, {handle});

  auto &component = entityDatabase.get<quoll::Script>(entity);

  scriptingSystem.start(entityDatabase);

  EXPECT_LT(component.onCollisionStart, quoll::EventObserverMax);
  EXPECT_LT(component.onCollisionEnd, quoll::EventObserverMax);
  EXPECT_LT(component.onKeyPress, quoll::EventObserverMax);
  EXPECT_LT(component.onKeyRelease, quoll::EventObserverMax);
}

TEST_F(ScriptingSystemTest,
       DoesNotCallScriptCollisionEventIfEntityDidNotCollide) {
  auto handle = loadLuaScript();

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Script>(entity, {handle});

  auto &component = entityDatabase.get<quoll::Script>(entity);

  scriptingSystem.start(entityDatabase);

  eventSystem.dispatch(quoll::CollisionEvent::CollisionStarted,
                       {quoll::Entity{5}, quoll::Entity{6}});
  eventSystem.poll();

  EXPECT_EQ(component.scope.getGlobal<int32_t>("event"), 0);
}

TEST_F(ScriptingSystemTest, CallsScriptCollisionStartEventIfEntityCollided) {
  auto handle = loadLuaScript();

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Script>(entity, {handle});

  auto &component = entityDatabase.get<quoll::Script>(entity);

  scriptingSystem.start(entityDatabase);

  eventSystem.dispatch(quoll::CollisionEvent::CollisionStarted,
                       {entity, quoll::Entity{6}});
  eventSystem.poll();

  auto *luaScope = component.scope.getLuaState();
  EXPECT_EQ(component.scope.getGlobal<int32_t>("event"), 1);
  EXPECT_EQ(component.scope.getGlobal<int32_t>("target"), 6);
}

TEST_F(ScriptingSystemTest, CallsScriptCollisionEndEventIfEntityCollided) {
  auto handle = loadLuaScript();

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Script>(entity, {handle});

  auto &component = entityDatabase.get<quoll::Script>(entity);

  scriptingSystem.start(entityDatabase);

  eventSystem.dispatch(quoll::CollisionEvent::CollisionEnded,
                       {quoll::Entity{5}, entity});
  eventSystem.poll();

  EXPECT_EQ(component.scope.getGlobal<int32_t>("event"), 2);
  EXPECT_EQ(component.scope.getGlobal<int32_t>("target"), 5);
}

TEST_F(ScriptingSystemTest, CallsScriptKeyPressEventIfKeyIsPressed) {
  auto handle = loadLuaScript();

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Script>(entity, {handle});

  auto &component = entityDatabase.get<quoll::Script>(entity);

  scriptingSystem.start(entityDatabase);

  eventSystem.dispatch(quoll::KeyboardEvent::Pressed, {15, -1, 3});
  eventSystem.poll();

  auto *luaScope = component.scope.getLuaState();
  EXPECT_EQ(component.scope.getGlobal<int32_t>("event"), 3);
  EXPECT_EQ(component.scope.getGlobal<int32_t>("key_value"), 15);
  EXPECT_EQ(component.scope.getGlobal<int32_t>("key_mods"), 3);
}

TEST_F(ScriptingSystemTest, CallsScriptKeyReleaseEventIfKeyIsReleased) {
  auto handle = loadLuaScript();

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Script>(entity, {handle});

  auto &component = entityDatabase.get<quoll::Script>(entity);

  scriptingSystem.start(entityDatabase);

  eventSystem.dispatch(quoll::KeyboardEvent::Released, {35, -1, 3});
  eventSystem.poll();

  EXPECT_EQ(component.scope.getGlobal<int32_t>("event"), 4);
  EXPECT_EQ(component.scope.getGlobal<int32_t>("key_value"), 35);
  EXPECT_EQ(component.scope.getGlobal<int32_t>("key_mods"), 3);
}
