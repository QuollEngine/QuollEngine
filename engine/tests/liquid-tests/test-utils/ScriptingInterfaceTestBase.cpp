#include "liquid/core/Base.h"
#include "liquid-tests/Testing.h"

#include "ScriptingInterfaceTestBase.h"

const liquid::String LuaScriptingInterfaceTestBase::ScriptName =
    "scripting-system-component-tester.lua";

LuaScriptingInterfaceTestBase::LuaScriptingInterfaceTestBase(
    const liquid::String &scriptName)
    : assetCache(std::filesystem::current_path()),
      scriptingSystem(eventSystem, assetCache.getRegistry()),
      mScriptName(scriptName) {}

liquid::LuaScope &
LuaScriptingInterfaceTestBase::call(liquid::Entity entity,
                                    const liquid::String &functionName) {
  auto uuid = liquid::Uuid::generate();
  assetCache.createLuaScriptFromSource(FixturesPath / mScriptName, uuid);
  auto handle = assetCache.loadLuaScript(uuid).getData();

  entityDatabase.set<liquid::Script>(entity, {handle});

  scriptingSystem.start(entityDatabase);

  auto &scripting = entityDatabase.get<liquid::Script>(entity);

  scripting.scope.luaGetGlobal(functionName);
  scripting.scope.call(0);

  return scripting.scope;
}
