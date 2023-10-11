#include "quoll/core/Base.h"
#include "quoll/asset/AssetCache.h"
#include "quoll/scripting/ScriptingSystem.h"
#include "quoll/scripting/EntityTable.h"

#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/AssetCacheTestBase.h"
#include "quoll-tests/test-utils/TestPhysicsBackend.h"

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
  quoll::PhysicsSystem physicsSystem{new TestPhysicsBackend};
};

using ScriptingSystemDeathTest = ScriptingSystemTest;

static constexpr float TimeDelta = 0.2f;

TEST_F(ScriptingSystemTest, CallsScriptingUpdateFunctionOnUpdate) {
  auto handle = loadLuaScript();

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Script>(entity, {handle});

  auto &component = entityDatabase.get<quoll::Script>(entity);
  EXPECT_EQ(component.state, nullptr);

  scriptingSystem.start(entityDatabase, physicsSystem);
  EXPECT_NE(component.state, nullptr);

  scriptingSystem.update(TimeDelta, entityDatabase);

  sol::state_view state(component.state);
  EXPECT_EQ(state["value"].get<int32_t>(), 0);
  EXPECT_EQ(state["global_dt"].get<float>(), TimeDelta);
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

  scriptingSystem.start(entityDatabase, physicsSystem);

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
  EXPECT_EQ(component.state, nullptr);

  scriptingSystem.start(entityDatabase, physicsSystem);
  EXPECT_NE(component.state, nullptr);

  for (size_t i = 0; i < 10; ++i) {
    scriptingSystem.update(TimeDelta, entityDatabase);
  }

  sol::state_view state(component.state);
  EXPECT_EQ(state["value"].get<int32_t>(), 9);
}

TEST_F(ScriptingSystemTest, CallsScriptStartFunctionOnStart) {
  auto handle = loadLuaScript();

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Script>(entity, {handle});

  auto &component = entityDatabase.get<quoll::Script>(entity);
  EXPECT_EQ(component.state, nullptr);

  scriptingSystem.start(entityDatabase, physicsSystem);
  EXPECT_NE(component.state, nullptr);

  sol::state_view state(component.state);
  EXPECT_EQ(state["value"].get<int32_t>(), -1);
}

TEST_F(ScriptingSystemTest, CallsScriptingStartFunctionOnlyOnceOnStart) {
  auto handle = loadLuaScript();

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Script>(entity, {handle});

  auto &component = entityDatabase.get<quoll::Script>(entity);
  EXPECT_EQ(component.state, nullptr);

  // Call 10 times
  for (size_t i = 0; i < 10; ++i) {
    scriptingSystem.start(entityDatabase, physicsSystem);
  }
  EXPECT_NE(component.state, nullptr);

  auto state = sol::state_view(component.state);
  EXPECT_EQ(state["value"].get<int32_t>(), -1);
}

TEST_F(ScriptingSystemTest, RemovesScriptComponentIfInputVarsAreNotSet) {
  auto handle = loadLuaScript("scripting-system-vars.lua");

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Script>(entity, {handle});

  auto &component = entityDatabase.get<quoll::Script>(entity);

  scriptingSystem.start(entityDatabase, physicsSystem);
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

  scriptingSystem.start(entityDatabase, physicsSystem);
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

  scriptingSystem.start(entityDatabase, physicsSystem);
  ASSERT_TRUE(entityDatabase.has<quoll::Script>(entity));

  auto state = sol::state_view(component.state);

  EXPECT_EQ(state["var_string"].get<quoll::String>(), "Hello world");
  EXPECT_EQ(state["var_prefab"].get<uint32_t>(), 15);
  EXPECT_EQ(state["var_texture"].get<uint32_t>(), 25);
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

  scriptingSystem.start(entityDatabase, physicsSystem);
  auto state = sol::state_view(component.state);

  EXPECT_EQ(state["var_string"].get<quoll::String>(), "Hello world");
  EXPECT_EQ(state["var_prefab"].get<uint32_t>(), 15);
  EXPECT_EQ(state["var_texture"].get<uint32_t>(), 25);

  EXPECT_TRUE(state["global_vars"].get_type() == sol::type::table);
  EXPECT_TRUE(state["entity"].is<quoll::EntityTable>());

  state["update"]();

  EXPECT_TRUE(state["global_vars"].is<sol::nil_t>());
  EXPECT_TRUE(state["entity"].is<quoll::EntityTable>());
}

TEST_F(ScriptingSystemTest, RegistersEventsOnStart) {
  auto handle = loadLuaScript();

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Script>(entity, {handle});

  auto &component = entityDatabase.get<quoll::Script>(entity);

  scriptingSystem.start(entityDatabase, physicsSystem);

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

  scriptingSystem.start(entityDatabase, physicsSystem);
  auto state = sol::state_view(component.state);

  eventSystem.dispatch(quoll::CollisionEvent::CollisionStarted,
                       {quoll::Entity{5}, quoll::Entity{6}});
  eventSystem.poll();

  EXPECT_EQ(state["event"].get<int32_t>(), 0);
}

TEST_F(ScriptingSystemTest, CallsScriptCollisionStartEventIfEntityCollided) {
  auto handle = loadLuaScript();

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Script>(entity, {handle});

  auto &component = entityDatabase.get<quoll::Script>(entity);

  scriptingSystem.start(entityDatabase, physicsSystem);
  auto state = sol::state_view(component.state);

  eventSystem.dispatch(quoll::CollisionEvent::CollisionStarted,
                       {entity, quoll::Entity{6}});
  eventSystem.poll();

  EXPECT_EQ(state["event"].get<int32_t>(), 1);
  EXPECT_EQ(state["target"].get<int32_t>(), 6);
}

TEST_F(ScriptingSystemTest, CallsScriptCollisionEndEventIfEntityCollided) {
  auto handle = loadLuaScript();

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Script>(entity, {handle});

  auto &component = entityDatabase.get<quoll::Script>(entity);

  scriptingSystem.start(entityDatabase, physicsSystem);
  auto state = sol::state_view(component.state);

  eventSystem.dispatch(quoll::CollisionEvent::CollisionEnded,
                       {quoll::Entity{5}, entity});
  eventSystem.poll();

  EXPECT_EQ(state["event"].get<int32_t>(), 2);
  EXPECT_EQ(state["target"].get<int32_t>(), 5);
}

TEST_F(ScriptingSystemTest, CallsScriptKeyPressEventIfKeyIsPressed) {
  auto handle = loadLuaScript();

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Script>(entity, {handle});

  auto &component = entityDatabase.get<quoll::Script>(entity);

  scriptingSystem.start(entityDatabase, physicsSystem);
  auto state = sol::state_view(component.state);

  eventSystem.dispatch(quoll::KeyboardEvent::Pressed, {15, -1, 3});
  eventSystem.poll();

  EXPECT_EQ(state["event"].get<int32_t>(), 3);
  EXPECT_EQ(state["key_value"].get<int32_t>(), 15);
  EXPECT_EQ(state["key_mods"].get<int32_t>(), 3);
}

TEST_F(ScriptingSystemTest, CallsScriptKeyReleaseEventIfKeyIsReleased) {
  auto handle = loadLuaScript();

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Script>(entity, {handle});

  auto &component = entityDatabase.get<quoll::Script>(entity);

  scriptingSystem.start(entityDatabase, physicsSystem);
  auto state = sol::state_view(component.state);

  eventSystem.dispatch(quoll::KeyboardEvent::Released, {35, -1, 3});
  eventSystem.poll();

  EXPECT_EQ(state["event"].get<int32_t>(), 4);
  EXPECT_EQ(state["key_value"].get<int32_t>(), 35);
  EXPECT_EQ(state["key_mods"].get<int32_t>(), 3);
}
