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
  LuaScriptingSystemTest() : scriptingSystem(cache.getRegistry()) {
    scriptingSystem.createSystemViewData(view);
  }

  quoll::LuaScriptAssetHandle loadLuaScript(quoll::String filename) {
    auto uuid = quoll::Uuid::generate();
    cache.createLuaScriptFromSource(FixturesPath / filename, uuid);
    return cache.loadLuaScript(uuid).getData();
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

  auto entity = entityDatabase.entity();
  entity.set<quoll::LuaScript>({handle});

  auto component = entity.get_ref<quoll::LuaScript>();
  EXPECT_EQ(component->state, nullptr);

  scriptingSystem.start(view, physicsSystem, windowSignals);
  EXPECT_NE(component->state, nullptr);

  scriptingSystem.update(TimeDelta, view);

  sol::state_view state(component->state);
  EXPECT_EQ(state["value"].get<i32>(), 0);
  EXPECT_EQ(state["global_dt"].get<f32>(), TimeDelta);
}

TEST_F(LuaScriptingSystemTest, DeletesScriptDataWhenComponentIsDeleted) {
  auto handle = loadLuaScript("scripting-system-tester.lua");

  static constexpr usize NumEntities = 20;

  std::vector<quoll::Entity> entities(NumEntities);
  for (usize i = 0; i < entities.size(); ++i) {
    auto entity = entityDatabase.entity();
    entities.at(i) = entity;

    entity.set<quoll::LuaScript>({handle});
  }

  scriptingSystem.start(view, physicsSystem, windowSignals);

  std::vector<flecs::ref<quoll::LuaScript>> scripts(entities.size());
  for (usize i = 0; i < entities.size(); ++i) {
    auto entity = entities.at(i);
    scripts.at(i) = entity.get_ref<quoll::LuaScript>();

    if ((i % 2) == 0) {
      entity.remove<quoll::LuaScript>();
    }
  }

  scriptingSystem.update(TimeDelta, view);
  for (usize i = 0; i < entities.size(); ++i) {
    auto entity = entities.at(i);
    bool deleted = (i % 2) == 0;
    EXPECT_NE(entity.has<quoll::LuaScript>(), deleted);
  }
}

TEST_F(LuaScriptingSystemTest, DoesNothingIfScriptHasNoUpdater) {
  auto handle = loadLuaScript("scripting-system-tester.lua");

  auto entity = entityDatabase.entity();
  entity.set<quoll::LuaScript>({handle});

  auto component = entity.get_ref<quoll::LuaScript>();
  EXPECT_EQ(component->state, nullptr);

  scriptingSystem.start(view, physicsSystem, windowSignals);
  EXPECT_NE(component->state, nullptr);

  sol::state_view state(component->state);
  state["disconnect_updater"]();

  for (usize i = 0; i < 10; ++i) {
    scriptingSystem.update(TimeDelta, view);
  }

  EXPECT_EQ(state["value"].get<i32>(), -1);
}

TEST_F(LuaScriptingSystemTest, CallsScriptingUpdateFunctionOnEveryUpdate) {
  auto handle = loadLuaScript("scripting-system-tester.lua");

  auto entity = entityDatabase.entity();
  entity.set<quoll::LuaScript>({handle});

  auto component = entity.get_ref<quoll::LuaScript>();
  EXPECT_EQ(component->state, nullptr);

  scriptingSystem.start(view, physicsSystem, windowSignals);
  EXPECT_NE(component->state, nullptr);

  for (usize i = 0; i < 10; ++i) {
    scriptingSystem.update(TimeDelta, view);
  }

  sol::state_view state(component->state);
  EXPECT_EQ(state["value"].get<i32>(), 9);
}

TEST_F(LuaScriptingSystemTest, LoadsScriptOnStart) {
  auto handle = loadLuaScript("scripting-system-tester.lua");

  auto entity = entityDatabase.entity();
  entity.set<quoll::LuaScript>({handle});

  auto component = entity.get_ref<quoll::LuaScript>();
  EXPECT_EQ(component->state, nullptr);

  scriptingSystem.start(view, physicsSystem, windowSignals);
  EXPECT_NE(component->state, nullptr);

  sol::state_view state(component->state);
  EXPECT_EQ(state["value"].get<i32>(), -1);
}

TEST_F(LuaScriptingSystemTest, LoadsScriptOnlyOnceOnStart) {
  auto handle = loadLuaScript("scripting-system-tester.lua");

  auto entity = entityDatabase.entity();
  entity.set<quoll::LuaScript>({handle});

  auto component = entity.get_ref<quoll::LuaScript>();
  EXPECT_EQ(component->state, nullptr);

  // Call 10 times
  for (usize i = 0; i < 10; ++i) {
    scriptingSystem.start(view, physicsSystem, windowSignals);
  }
  EXPECT_NE(component->state, nullptr);

  auto state = sol::state_view(component->state);
  EXPECT_EQ(state["value"].get<i32>(), -1);
}

TEST_F(LuaScriptingSystemTest, RemovesScriptComponentIfInputVarsAreNotSet) {
  auto handle = loadLuaScript("scripting-system-vars.lua");

  auto entity = entityDatabase.entity();
  entity.set<quoll::LuaScript>({handle});

  auto component = entity.get_ref<quoll::LuaScript>();

  scriptingSystem.start(view, physicsSystem, windowSignals);
  EXPECT_FALSE(entity.has<quoll::LuaScript>());
}

TEST_F(LuaScriptingSystemTest,
       RemovesScriptComponentIfInputVarTypesAreInvalid) {
  auto handle = loadLuaScript("scripting-system-vars.lua");

  auto entity = entityDatabase.entity();
  quoll::LuaScript script{handle};
  script.variables.insert_or_assign("string_value",
                                    quoll::PrefabAssetHandle{15});
  script.variables.insert_or_assign("prefab_value",
                                    quoll::PrefabAssetHandle{15});
  script.variables.insert_or_assign("texture_value",
                                    quoll::TextureAssetHandle{25});
  entity.set(script);

  auto component = entity.get_ref<quoll::LuaScript>();

  scriptingSystem.start(view, physicsSystem, windowSignals);
  EXPECT_FALSE(entity.has<quoll::LuaScript>());
}

TEST_F(LuaScriptingSystemTest, SetsVariablesToInputVarsOnStart) {
  auto handle = loadLuaScript("scripting-system-vars.lua");

  auto entity = entityDatabase.entity();
  quoll::LuaScript script{handle};
  script.variables.insert_or_assign("string_value",
                                    quoll::String("Hello world"));
  script.variables.insert_or_assign("prefab_value",
                                    quoll::PrefabAssetHandle{15});
  script.variables.insert_or_assign("texture_value",
                                    quoll::TextureAssetHandle{25});
  entity.set(script);

  auto component = entity.get_ref<quoll::LuaScript>();

  scriptingSystem.start(view, physicsSystem, windowSignals);
  ASSERT_TRUE(entity.has<quoll::LuaScript>());

  auto state = sol::state_view(component->state);

  EXPECT_EQ(state["var_string"].get<quoll::String>(), "Hello world");
  EXPECT_EQ(state["var_prefab"].get<u32>(), 15);
  EXPECT_EQ(state["var_texture"].get<u32>(), 25);
}

TEST_F(LuaScriptingSystemTest, RemovesVariableSetterAfterInputVariablesAreSet) {
  auto handle = loadLuaScript("scripting-system-vars.lua");

  auto entity = entityDatabase.entity();
  quoll::LuaScript script{handle};
  script.variables.insert_or_assign("string_value",
                                    quoll::String("Hello world"));
  script.variables.insert_or_assign("prefab_value",
                                    quoll::PrefabAssetHandle{15});
  script.variables.insert_or_assign("texture_value",
                                    quoll::TextureAssetHandle{25});
  entity.set(script);

  auto component = entity.get_ref<quoll::LuaScript>();

  scriptingSystem.start(view, physicsSystem, windowSignals);
  auto state = sol::state_view(component->state);

  EXPECT_EQ(state["var_string"].get<quoll::String>(), "Hello world");
  EXPECT_EQ(state["var_prefab"].get<u32>(), 15);
  EXPECT_EQ(state["var_texture"].get<u32>(), 25);

  EXPECT_TRUE(state["global_vars"].get_type() == sol::type::table);
  EXPECT_TRUE(state["entity"].is<quoll::EntityLuaTable>());

  state["update"]();

  EXPECT_TRUE(state["global_vars"].is<sol::nil_t>());
  EXPECT_TRUE(state["entity"].is<quoll::EntityLuaTable>());
}
