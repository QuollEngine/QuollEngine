#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "quoll/core/Profiler.h"
#include "quoll/asset/AssetRegistry.h"
#include "quoll/entity/EntityDatabase.h"
#include "quoll/scene/Scene.h"
#include "quoll/system/SystemView.h"
#include "LuaScriptingSystem.h"
#include "ScriptDecorator.h"

namespace quoll {

LuaScriptingSystem::LuaScriptingSystem(AssetRegistry &assetRegistry)
    : mAssetRegistry(assetRegistry) {}

void LuaScriptingSystem::start(SystemView &view, PhysicsSystem &physicsSystem,
                               WindowSignals &windowSignals) {
  auto &entityDatabase = view.scene->entityDatabase;

  ScriptGlobals scriptGlobals{windowSignals, entityDatabase, physicsSystem,
                              mAssetRegistry, mScriptLoop};
  QUOLL_PROFILE_EVENT("LuaScriptingSystem::start");
  lua::ScriptDecorator scriptDecorator;
  std::vector<Entity> deleteList;
  std::vector<lua::DeferredLoader *> loaders;
  for (auto [entity, component] : entityDatabase.view<LuaScript>()) {
    if (component.started) {
      continue;
    }

    bool valid = true;
    auto &script = mAssetRegistry.get(component.handle);
    for (auto &[key, value] : script.variables) {
      auto it = component.variables.find(key);
      if (it == component.variables.end() || !it->second.isType(value.type)) {
        // TODO: Throw error here
        valid = false;
        break;
      }
    }

    if (!valid) {
      deleteList.push_back(entity);
      continue;
    }

    component.loader = [this, scriptGlobals, &scriptDecorator, &script,
                        entity]() {
      auto &component = scriptGlobals.entityDatabase.get<LuaScript>(entity);
      component.started = true;

      if (component.state) {
        mLuaInterpreter.destroyState(component.state);
      }
      component.state = mLuaInterpreter.createState();
      auto state = sol::state_view(component.state);

      scriptDecorator.attachToScope(state, entity, scriptGlobals);
      scriptDecorator.attachVariableInjectors(state, component.variables);

      mLuaInterpreter.evaluate(script.bytes, component.state);
      scriptDecorator.removeVariableInjectors(state);
    };

    loaders.push_back(&component.loader);
  }

  for (auto *loader : loaders) {
    loader->wait();
  }

  for (auto entity : deleteList) {
    entityDatabase.remove<LuaScript>(entity);
  }
}

void LuaScriptingSystem::update(f32 dt, SystemView &view) {
  QUOLL_PROFILE_EVENT("LuaScriptingSystem::update");

  for (auto [entity, script] : view.luaScripting.scriptRemoveObserver) {
    destroyScriptingData(script);
  }
  view.luaScripting.scriptRemoveObserver.clear();

  mScriptLoop.onUpdate().notify(dt);
}

void LuaScriptingSystem::cleanup(SystemView &view) {
  auto &entityDatabase = view.scene->entityDatabase;
  for (auto [entity, script] : entityDatabase.view<LuaScript>()) {
    destroyScriptingData(script);
  }

  entityDatabase.destroyComponents<LuaScript>();
}

void LuaScriptingSystem::createSystemViewData(SystemView &view) {
  view.luaScripting.scriptRemoveObserver =
      view.scene->entityDatabase.observeRemove<LuaScript>();
}

void LuaScriptingSystem::destroyScriptingData(LuaScript &component) {
  for (auto slot : component.signalSlots) {
    slot.disconnect();
  }

  if (component.state) {
    mLuaInterpreter.destroyState(component.state);
  }
}

} // namespace quoll
