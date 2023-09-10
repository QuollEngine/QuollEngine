#include "quoll/core/Base.h"
#include "quoll-tests/Testing.h"

#include "ScriptingInterfaceTestBase.h"

const quoll::String LuaScriptingInterfaceTestBase::ScriptName =
    "scripting-system-component-tester.lua";

LuaScriptingInterfaceTestBase::LuaScriptingInterfaceTestBase(
    const quoll::String &scriptName)
    : assetCache(std::filesystem::current_path()),
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
  scripting.scope.call(0);

  return scripting.scope;
}