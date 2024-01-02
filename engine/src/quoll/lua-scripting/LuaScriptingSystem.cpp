#include "quoll/core/Base.h"
#include "LuaScriptingSystem.h"
#include "ScriptDecorator.h"

#include "quoll/core/Engine.h"

namespace quoll {

LuaScriptingSystem::LuaScriptingSystem(AssetRegistry &assetRegistry)
    : mAssetRegistry(assetRegistry) {}

void LuaScriptingSystem::start(EntityDatabase &entityDatabase,
                               PhysicsSystem &physicsSystem,
                               WindowSignals &windowSignals) {
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
    auto &script = mAssetRegistry.getLuaScripts().getAsset(component.handle);
    for (auto &[key, value] : script.data.variables) {
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

      mLuaInterpreter.evaluate(script.data.bytes, component.state);
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

void LuaScriptingSystem::update(f32 dt, EntityDatabase &entityDatabase) {
  QUOLL_PROFILE_EVENT("LuaScriptingSystem::update");

  for (auto [entity, script] : mScriptRemoveObserver) {
    destroyScriptingData(script);
  }
  mScriptRemoveObserver.clear();

  mScriptLoop.onUpdate().notify(dt);
}

void LuaScriptingSystem::cleanup(EntityDatabase &entityDatabase) {
  for (auto [entity, script] : entityDatabase.view<LuaScript>()) {
    destroyScriptingData(script);
  }

  entityDatabase.destroyComponents<LuaScript>();
}

void LuaScriptingSystem::observeChanges(EntityDatabase &entityDatabase) {
  mScriptRemoveObserver = entityDatabase.observeRemove<LuaScript>();
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
