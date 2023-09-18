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
      mScriptName(scriptName) {}

quoll::LuaScope &
LuaScriptingInterfaceTestBase::call(quoll::Entity entity,
                                    const quoll::String &functionName) {
  auto uuid = quoll::Uuid::generate();
  assetCache.createLuaScriptFromSource(FixturesPath / mScriptName, uuid);
  auto handle = assetCache.loadLuaScript(uuid).getData();

  entityDatabase.set<quoll::Script>(entity, {handle});

  scriptingSystem.start(entityDatabase);

  auto &scripting = entityDatabase.get<quoll::Script>(entity);

  scripting.scope.luaGetGlobal(functionName);
  if (!scripting.scope.call(0)) {
    QuollAssert(false, "Failed to call function: " + functionName);
  }

  return scripting.scope;
}

void LuaScriptingInterfaceTestBase::SetUp() {
  TearDown();
  std::filesystem::create_directory(CachePath);
}

void LuaScriptingInterfaceTestBase::TearDown() {
  std::filesystem::remove_all(CachePath);
}
