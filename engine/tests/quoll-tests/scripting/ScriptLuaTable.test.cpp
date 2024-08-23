#include "quoll/core/Base.h"
#include "quoll/core/Name.h"
#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

using ScriptLuaTableTest = LuaScriptingInterfaceTestBase;

TEST_F(ScriptLuaTableTest, GetReturnsScriptVariableIfExistsInScript) {
  auto handleSource = loadScript("entity-script-load-tester-source.lua");
  auto handleTarget = loadScript("entity-script-load-tester-target.lua");

  auto target = entityDatabase.entity();
  target.set<quoll::Name>({"target"});
  target.set<quoll::LuaScript>({handleTarget});

  auto source = entityDatabase.entity();
  source.set<quoll::LuaScript>({handleSource});

  scriptingSystem.start(view, physicsSystem, windowSignals);

  sol::state_view state(source.get_ref<quoll::LuaScript>()->state);

  EXPECT_EQ(state["retrievedScriptValue"].get<u32>(), 10);
}

TEST_F(ScriptLuaTableTest, GetReturnsNilIfVariableDoesNotExistInScript) {
  auto handleSource = loadScript("entity-script-load-tester-source.lua");
  auto handleTarget = loadScript("entity-script-load-tester-target.lua");

  auto target = entityDatabase.entity();
  target.set<quoll::Name>({"target"});
  target.set<quoll::LuaScript>({handleTarget});

  auto source = entityDatabase.entity();
  source.set<quoll::LuaScript>({handleSource});

  scriptingSystem.start(view, physicsSystem, windowSignals);

  sol::state_view state(source.get_ref<quoll::LuaScript>()->state);

  EXPECT_TRUE(state["nonExistentScriptValue"].is<sol::nil_t>());
}

TEST_F(ScriptLuaTableTest, SetAssignsValueToTargetScript) {
  auto handleSource = loadScript("entity-script-load-tester-source.lua");
  auto handleTarget = loadScript("entity-script-load-tester-target.lua");

  auto target = entityDatabase.entity();
  target.set<quoll::Name>({"target"});
  target.set<quoll::LuaScript>({handleTarget});

  auto source = entityDatabase.entity();
  source.set<quoll::LuaScript>({handleSource});

  scriptingSystem.start(view, physicsSystem, windowSignals);

  sol::state_view state(target.get_ref<quoll::LuaScript>()->state);

  EXPECT_EQ(state["scriptAnotherValue"].get<quoll::String>(), "yes");
}

TEST_F(ScriptLuaTableTest,
       WaitsForTargetScriptToBeLoadedBeforeLoadingSourceScript) {
  auto handleSource = loadScript("entity-script-load-tester-source.lua");
  auto handleTarget = loadScript("entity-script-load-tester-target.lua");

  // Source is intentionally added first
  auto source = entityDatabase.entity();
  source.set<quoll::LuaScript>({handleSource});

  auto target = entityDatabase.entity();
  target.set<quoll::Name>({"target"});
  target.set<quoll::LuaScript>({handleTarget});

  scriptingSystem.start(view, physicsSystem, windowSignals);

  sol::state_view sourceState(source.get_ref<quoll::LuaScript>()->state);

  EXPECT_EQ(sourceState["retrievedScriptValue"].get<u32>(), 10);

  sol::state_view targetState(target.get_ref<quoll::LuaScript>()->state);
  EXPECT_EQ(targetState["scriptAnotherValue"].get<quoll::String>(), "yes");
}
