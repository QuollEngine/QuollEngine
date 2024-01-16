#include "quoll/core/Base.h"
#include "quoll/core/Name.h"
#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

using ScriptLuaTableTest = LuaScriptingInterfaceTestBase;

TEST_F(ScriptLuaTableTest, GetReturnsScriptVariableIfExistsInScript) {
  auto handleSource = loadScript("entity-script-load-tester-source.lua");
  auto handleTarget = loadScript("entity-script-load-tester-target.lua");

  auto target = entityDatabase.create();
  entityDatabase.set<quoll::Name>(target, {"target"});
  entityDatabase.set<quoll::LuaScript>(target, {handleTarget});

  auto source = entityDatabase.create();
  entityDatabase.set<quoll::LuaScript>(source, {handleSource});

  scriptingSystem.start(entityDatabase, physicsSystem, windowSignals);

  sol::state_view state(entityDatabase.get<quoll::LuaScript>(source).state);

  EXPECT_EQ(state["retrievedScriptValue"].get<u32>(), 10);
}

TEST_F(ScriptLuaTableTest, GetReturnsNilIfVariableDoesNotExistInScript) {
  auto handleSource = loadScript("entity-script-load-tester-source.lua");
  auto handleTarget = loadScript("entity-script-load-tester-target.lua");

  auto target = entityDatabase.create();
  entityDatabase.set<quoll::Name>(target, {"target"});
  entityDatabase.set<quoll::LuaScript>(target, {handleTarget});

  auto source = entityDatabase.create();
  entityDatabase.set<quoll::LuaScript>(source, {handleSource});

  scriptingSystem.start(entityDatabase, physicsSystem, windowSignals);

  sol::state_view state(entityDatabase.get<quoll::LuaScript>(source).state);

  EXPECT_TRUE(state["nonExistentScriptValue"].is<sol::nil_t>());
}

TEST_F(ScriptLuaTableTest, SetAssignsValueToTargetScript) {
  auto handleSource = loadScript("entity-script-load-tester-source.lua");
  auto handleTarget = loadScript("entity-script-load-tester-target.lua");

  auto target = entityDatabase.create();
  entityDatabase.set<quoll::Name>(target, {"target"});
  entityDatabase.set<quoll::LuaScript>(target, {handleTarget});

  auto source = entityDatabase.create();
  entityDatabase.set<quoll::LuaScript>(source, {handleSource});

  scriptingSystem.start(entityDatabase, physicsSystem, windowSignals);

  sol::state_view state(entityDatabase.get<quoll::LuaScript>(target).state);

  EXPECT_EQ(state["scriptAnotherValue"].get<quoll::String>(), "yes");
}

TEST_F(ScriptLuaTableTest,
       WaitsForTargetScriptToBeLoadedBeforeLoadingSourceScript) {
  auto handleSource = loadScript("entity-script-load-tester-source.lua");
  auto handleTarget = loadScript("entity-script-load-tester-target.lua");

  // Source is intentionally added first
  auto source = entityDatabase.create();
  entityDatabase.set<quoll::LuaScript>(source, {handleSource});

  auto target = entityDatabase.create();
  entityDatabase.set<quoll::Name>(target, {"target"});
  entityDatabase.set<quoll::LuaScript>(target, {handleTarget});

  scriptingSystem.start(entityDatabase, physicsSystem, windowSignals);

  sol::state_view sourceState(
      entityDatabase.get<quoll::LuaScript>(source).state);

  EXPECT_EQ(sourceState["retrievedScriptValue"].get<u32>(), 10);

  sol::state_view targetState(
      entityDatabase.get<quoll::LuaScript>(target).state);
  EXPECT_EQ(targetState["scriptAnotherValue"].get<quoll::String>(), "yes");
}
