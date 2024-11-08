#include "quoll/core/Base.h"
#include "quoll/asset/AssetCache.h"
#include "quoll/entity/EntityLuaTable.h"
#include "quoll/lua-scripting/LuaScriptingSystem.h"
#include "quoll/physics/PhysicsSystem.h"
#include "quoll/system/SystemView.h"
#include "quoll/window/WindowSignals.h"
#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/AssetCacheTestBase.h"
#include "quoll-tests/test-utils/TestPhysicsBackend.h"

class LuaScriptingSystemTest : public AssetCacheTestBase {
public:
  LuaScriptingSystemTest() : scriptingSystem(cache) {
    scriptingSystem.createSystemViewData(view);
  }

  quoll::AssetRef<quoll::LuaScriptAsset> loadLuaScript(quoll::String filename) {
    auto uuid = quoll::Uuid::generate();
    cache.createFromSource<quoll::LuaScriptAsset>(FixturesPath / filename,
                                                  uuid);
    return requestAndWait<quoll::LuaScriptAsset>(uuid);
  }

  quoll::Scene scene;
  quoll::EntityDatabase &entityDatabase = scene.entityDatabase;
  quoll::SystemView view{&scene};
  quoll::LuaScriptingSystem scriptingSystem;
  quoll::PhysicsSystem physicsSystem{new TestPhysicsBackend};
  quoll::WindowSignals windowSignals;
};

using ScriptingSystemDeathTest = LuaScriptingSystemTest;

static constexpr f32 TimeDelta = 0.2f;

TEST_F(LuaScriptingSystemTest, CallsScriptingUpdateFunctionOnUpdate) {
  auto handle = loadLuaScript("scripting-system-tester.lua");

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::LuaScriptAssetRef>(entity, {handle});

  scriptingSystem.start(view, physicsSystem, windowSignals);
  auto &component = entityDatabase.get<quoll::LuaScript>(entity);
  ASSERT_NE(component.state, nullptr);

  scriptingSystem.update(TimeDelta, view);

  sol::state_view state(component.state);
  EXPECT_EQ(state["value"].get<i32>(), 0);
  EXPECT_EQ(state["global_dt"].get<f32>(), TimeDelta);
}

TEST_F(LuaScriptingSystemTest, DeletesScriptDataWhenComponentIsDeleted) {
  auto handle = loadLuaScript("scripting-system-tester.lua");

  static constexpr usize NumEntities = 20;

  std::vector<quoll::Entity> entities(NumEntities, quoll::Entity::Null);
  for (usize i = 0; i < entities.size(); ++i) {
    auto entity = entityDatabase.create();
    entities.at(i) = entity;

    entityDatabase.set<quoll::LuaScriptAssetRef>(entity, {handle});
  }

  scriptingSystem.start(view, physicsSystem, windowSignals);

  for (usize i = 0; i < entities.size(); ++i) {
    auto entity = entities.at(i);

    if ((i % 2) == 0) {
      entityDatabase.remove<quoll::LuaScriptAssetRef>(entity);
    }
  }

  scriptingSystem.update(TimeDelta, view);
  for (usize i = 0; i < entities.size(); ++i) {
    auto entity = entities.at(i);
    bool deleted = (i % 2) == 0;
    EXPECT_NE(entityDatabase.has<quoll::LuaScriptAssetRef>(entity), deleted);
    EXPECT_NE(entityDatabase.has<quoll::LuaScriptCurrentAsset>(entity),
              deleted);
    EXPECT_NE(entityDatabase.has<quoll::LuaScript>(entity), deleted);
  }
}

TEST_F(LuaScriptingSystemTest, DoesNothingIfScriptHasNoUpdater) {
  auto handle = loadLuaScript("scripting-system-tester.lua");

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::LuaScriptAssetRef>(entity, {handle});

  scriptingSystem.start(view, physicsSystem, windowSignals);

  auto &component = entityDatabase.get<quoll::LuaScript>(entity);
  ASSERT_NE(component.state, nullptr);

  sol::state_view state(component.state);
  state["disconnect_updater"]();

  for (usize i = 0; i < 10; ++i) {
    scriptingSystem.update(TimeDelta, view);
  }

  EXPECT_EQ(state["value"].get<i32>(), -1);
}

TEST_F(LuaScriptingSystemTest, CallsScriptingUpdateFunctionOnEveryUpdate) {
  auto handle = loadLuaScript("scripting-system-tester.lua");

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::LuaScriptAssetRef>(entity, {handle});

  scriptingSystem.start(view, physicsSystem, windowSignals);

  auto &component = entityDatabase.get<quoll::LuaScript>(entity);
  ASSERT_NE(component.state, nullptr);

  for (usize i = 0; i < 10; ++i) {
    scriptingSystem.update(TimeDelta, view);
  }

  sol::state_view state(component.state);
  EXPECT_EQ(state["value"].get<i32>(), 9);
}

TEST_F(LuaScriptingSystemTest, LoadsScriptOnStart) {
  auto handle = loadLuaScript("scripting-system-tester.lua");

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::LuaScriptAssetRef>(entity, {handle});

  scriptingSystem.start(view, physicsSystem, windowSignals);

  auto &component = entityDatabase.get<quoll::LuaScript>(entity);
  ASSERT_NE(component.state, nullptr);

  sol::state_view state(component.state);
  EXPECT_EQ(state["value"].get<i32>(), -1);
}

TEST_F(LuaScriptingSystemTest, LoadsScriptOnlyOnceOnStart) {
  auto handle = loadLuaScript("scripting-system-tester.lua");

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::LuaScriptAssetRef>(entity, {handle});

  // Call 10 times
  for (usize i = 0; i < 10; ++i) {
    scriptingSystem.start(view, physicsSystem, windowSignals);
  }
  auto &component = entityDatabase.get<quoll::LuaScript>(entity);
  ASSERT_NE(component.state, nullptr);

  auto state = sol::state_view(component.state);
  EXPECT_EQ(state["value"].get<i32>(), -1);
}

TEST_F(LuaScriptingSystemTest,
       DoesNotCreateScriptComponentIfInputVarsAreNotSet) {
  auto handle = loadLuaScript("scripting-system-vars.lua");

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::LuaScriptAssetRef>(entity, {handle});

  scriptingSystem.start(view, physicsSystem, windowSignals);
  EXPECT_FALSE(entityDatabase.has<quoll::LuaScript>(entity));
}

TEST_F(LuaScriptingSystemTest, DoesNotCreateScriptComponentIfValuesAreInvalid) {
  auto handle = loadLuaScript("scripting-system-vars.lua");

  auto prefab = createAsset<quoll::PrefabAsset>();
  auto texture = createAsset<quoll::TextureAsset>();

  auto entity = entityDatabase.create();
  quoll::LuaScriptAssetRef script{handle};
  script.variables.insert_or_assign("string_value", prefab);
  script.variables.insert_or_assign("prefab_value", prefab);
  script.variables.insert_or_assign("texture_value", texture);
  entityDatabase.set(entity, script);

  scriptingSystem.start(view, physicsSystem, windowSignals);
  EXPECT_FALSE(entityDatabase.has<quoll::LuaScript>(entity));
}

TEST_F(LuaScriptingSystemTest, SetsVariablesToInputVarsOnStart) {
  auto handle = loadLuaScript("scripting-system-vars.lua");

  auto prefab = createAsset<quoll::PrefabAsset>();
  auto texture = createAsset<quoll::TextureAsset>();

  auto entity = entityDatabase.create();
  quoll::LuaScriptAssetRef script{handle};
  script.variables.insert_or_assign("string_value",
                                    quoll::String("Hello world"));
  script.variables.insert_or_assign("prefab_value", prefab);
  script.variables.insert_or_assign("texture_value", texture);
  entityDatabase.set(entity, script);

  scriptingSystem.start(view, physicsSystem, windowSignals);
  ASSERT_TRUE(entityDatabase.has<quoll::LuaScript>(entity));
  auto &component = entityDatabase.get<quoll::LuaScript>(entity);

  ASSERT_NE(component.state, nullptr);
  auto state = sol::state_view(component.state);

  EXPECT_EQ(state["var_string"].get<quoll::String>(), "Hello world");
  EXPECT_EQ(state["var_prefab"].get<u32>(), prefab.handle().getRawId());
  EXPECT_EQ(state["var_texture"].get<u32>(), texture.handle().getRawId());
}

TEST_F(LuaScriptingSystemTest, RemovesVariableSetterAfterInputVariablesAreSet) {
  auto handle = loadLuaScript("scripting-system-vars.lua");

  auto prefab = createAsset<quoll::PrefabAsset>();
  auto texture = createAsset<quoll::TextureAsset>();

  auto entity = entityDatabase.create();
  quoll::LuaScriptAssetRef script{handle};
  script.variables.insert_or_assign("string_value",
                                    quoll::String("Hello world"));
  script.variables.insert_or_assign("prefab_value", prefab);
  script.variables.insert_or_assign("texture_value", texture);
  entityDatabase.set(entity, script);

  scriptingSystem.start(view, physicsSystem, windowSignals);
  ASSERT_TRUE(entityDatabase.has<quoll::LuaScript>(entity));
  auto &component = entityDatabase.get<quoll::LuaScript>(entity);

  ASSERT_NE(component.state, nullptr);
  auto state = sol::state_view(component.state);

  EXPECT_EQ(state["var_string"].get<quoll::String>(), "Hello world");
  EXPECT_EQ(state["var_prefab"].get<u32>(), prefab.handle().getRawId());
  EXPECT_EQ(state["var_texture"].get<u32>(), texture.handle().getRawId());

  EXPECT_TRUE(state["global_vars"].get_type() == sol::type::table);
  EXPECT_TRUE(state["entity"].is<quoll::EntityLuaTable>());

  state["update"]();

  EXPECT_TRUE(state["global_vars"].is<sol::nil_t>());
  EXPECT_TRUE(state["entity"].is<quoll::EntityLuaTable>());
}
