#include "liquid/core/Base.h"
#include "liquid-tests/Testing.h"

#include "ScriptingInterfaceTestBase.h"

const liquid::String LuaScriptingInterfaceTestBase::ScriptName =
    "scripting-system-component-tester.lua";

LuaScriptingInterfaceTestBase::LuaScriptingInterfaceTestBase(
    const liquid::String &scriptName)
    : assetManager(std::filesystem::current_path()),
      scriptingSystem(eventSystem, assetManager.getRegistry()),
      mScriptName(scriptName) {}

liquid::LuaScope &
LuaScriptingInterfaceTestBase::call(liquid::Entity entity,
                                    const liquid::String &functionName) {
  auto handle =
      assetManager
          .loadLuaScriptFromFile(std::filesystem::current_path() / mScriptName)
          .getData();

  entityDatabase.set<liquid::ScriptingComponent>(entity, {handle});

  scriptingSystem.start(entityDatabase);

  auto &scripting = entityDatabase.get<liquid::ScriptingComponent>(entity);

  scripting.scope.luaGetGlobal(functionName);
  scripting.scope.call(0);

  return scripting.scope;
}
