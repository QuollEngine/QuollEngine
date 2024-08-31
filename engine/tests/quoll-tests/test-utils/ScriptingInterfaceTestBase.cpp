#include "quoll/core/Base.h"
#include "quoll-tests/Testing.h"
#include "ScriptingInterfaceTestBase.h"

static const quoll::Path CachePath = std::filesystem::current_path() / "cache";

const quoll::String LuaScriptingInterfaceTestBase::ScriptName =
    "scripting-system-component-tester.lua";

LuaScriptingInterfaceTestBase::LuaScriptingInterfaceTestBase(
    const quoll::String &scriptName)
    : assetCache(CachePath), scriptingSystem(assetCache),
      mScriptName(scriptName), physicsSystem(physicsBackend) {}

sol::state_view LuaScriptingInterfaceTestBase::start(quoll::Entity entity) {
  auto handle = loadScript(mScriptName);
  entityDatabase.set<quoll::LuaScript>(entity, {handle});

  scriptingSystem.start(view, physicsSystem, windowSignals);

  auto &script = entityDatabase.get<quoll::LuaScript>(entity);
  sol::state_view state(script.state);

  state["assertNative"] = [](bool value) { return value; };

  return state;
}

sol::state_view
LuaScriptingInterfaceTestBase::call(quoll::Entity entity,
                                    const quoll::String &functionName) {
  auto state = start(entity);

  auto fnRes = state[functionName]();
  if (!fnRes.valid()) {
    sol::error error = fnRes;
    QuollAssert(false, error.what());
  }

  return state;
}

quoll::AssetRef<quoll::LuaScriptAsset>
LuaScriptingInterfaceTestBase::loadScript(quoll::String scriptName) {
  auto uuid = quoll::Uuid::generate();
  assetCache.createFromSource<quoll::LuaScriptAsset>(FixturesPath / scriptName,
                                                     uuid);

  auto res = assetCache.request<quoll::LuaScriptAsset>(uuid);
  QuollAssert(res, "Error loading script");
  return res;
}

void LuaScriptingInterfaceTestBase::SetUp() {
  TearDown();
  std::filesystem::create_directory(CachePath);
}

void LuaScriptingInterfaceTestBase::TearDown() {
  std::filesystem::remove_all(CachePath);
}
