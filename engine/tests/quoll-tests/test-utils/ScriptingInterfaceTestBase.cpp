#include "quoll/core/Base.h"
#include "quoll-tests/Testing.h"

#include "ScriptingInterfaceTestBase.h"

static const quoll::Path CachePath = std::filesystem::current_path() / "cache";

const quoll::String LuaScriptingInterfaceTestBase::ScriptName =
    "scripting-system-component-tester.lua";

LuaScriptingInterfaceTestBase::LuaScriptingInterfaceTestBase(
    const quoll::String &scriptName)
    : assetCache(CachePath),
      scriptingSystem(eventSystem, assetCache.getRegistry()),
      mScriptName(scriptName), physicsSystem(physicsBackend) {}

sol::state_view
LuaScriptingInterfaceTestBase::call(quoll::Entity entity,
                                    const quoll::String &functionName) {
  auto uuid = quoll::Uuid::generate();
  assetCache.createLuaScriptFromSource(FixturesPath / mScriptName, uuid);
  auto handle = assetCache.loadLuaScript(uuid).getData();

  entityDatabase.set<quoll::Script>(entity, {handle});

  scriptingSystem.start(entityDatabase, physicsSystem);

  auto &script = entityDatabase.get<quoll::Script>(entity);
  sol::state_view state(script.state);

  state["assert_native"] = [](bool value) { return value; };

  state[functionName]();

  return state;
}

void LuaScriptingInterfaceTestBase::SetUp() {
  TearDown();
  std::filesystem::create_directory(CachePath);
}

void LuaScriptingInterfaceTestBase::TearDown() {
  std::filesystem::remove_all(CachePath);
}
