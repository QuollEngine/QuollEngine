#include "quoll/core/Base.h"
#include "quoll/core/Name.h"

#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

using ScriptLuaTableTest = LuaScriptingInterfaceTestBase;

TEST_F(ScriptLuaTableTest, ReturnsScriptVariableIfExistsInScript) {
  auto handleSource = loadScript("entity-script-load-tester-source.lua");
  auto handleTarget = loadScript("entity-script-load-tester-target.lua");

  auto target = entityDatabase.create();
  entityDatabase.set<quoll::Name>(target, {"target"});
  entityDatabase.set<quoll::Script>(target, {handleTarget});

  auto source = entityDatabase.create();
  entityDatabase.set<quoll::Script>(source, {handleSource});

  scriptingSystem.start(entityDatabase, physicsSystem);

  sol::state_view state(entityDatabase.get<quoll::Script>(source).state);

  EXPECT_EQ(state["retrieved_script_value"].get<u32>(), 10);
}

TEST_F(ScriptLuaTableTest, ReturnsNilIfVariableDoesNotExistInScript) {
  auto handleSource = loadScript("entity-script-load-tester-source.lua");
  auto handleTarget = loadScript("entity-script-load-tester-target.lua");

  auto target = entityDatabase.create();
  entityDatabase.set<quoll::Name>(target, {"target"});
  entityDatabase.set<quoll::Script>(target, {handleTarget});

  auto source = entityDatabase.create();
  entityDatabase.set<quoll::Script>(source, {handleSource});

  scriptingSystem.start(entityDatabase, physicsSystem);

  sol::state_view state(entityDatabase.get<quoll::Script>(source).state);

  EXPECT_TRUE(state["non_existent_script_value"].is<sol::nil_t>());
}

TEST_F(ScriptLuaTableTest, WaitsForTargetScriptToBeLoadedBeforeLoadingScript) {
  auto handleSource = loadScript("entity-script-load-tester-source.lua");
  auto handleTarget = loadScript("entity-script-load-tester-target.lua");

  // Source is intentionally added first
  auto source = entityDatabase.create();
  entityDatabase.set<quoll::Script>(source, {handleSource});

  auto target = entityDatabase.create();
  entityDatabase.set<quoll::Name>(target, {"target"});
  entityDatabase.set<quoll::Script>(target, {handleTarget});

  scriptingSystem.start(entityDatabase, physicsSystem);

  sol::state_view state(entityDatabase.get<quoll::Script>(source).state);

  EXPECT_EQ(state["retrieved_script_value"].get<u32>(), 10);
}
