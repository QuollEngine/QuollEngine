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

  view.luaScripting.queryScripts.each([&](flecs::entity entity,
                                          LuaScript &component) {
    if (component.started) {
      return;
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
      return;
    }

    component.loader = [this, scriptGlobals, &scriptDecorator, &script,
                        entity]() {
      auto component = entity.get_ref<LuaScript>();
      component->started = true;

      if (component->state) {
        mLuaInterpreter.destroyState(component->state);
      }
      component->state = mLuaInterpreter.createState();
      auto state = sol::state_view(component->state);

      scriptDecorator.attachToScope(state, entity, scriptGlobals);
      scriptDecorator.attachVariableInjectors(state, component->variables);

      mLuaInterpreter.evaluate(script.data.bytes, component->state);
      scriptDecorator.removeVariableInjectors(state);
    };

    loaders.push_back(&component.loader);
  });

  for (auto *loader : loaders) {
    loader->wait();
  }

  for (auto entity : deleteList) {
    entity.remove<LuaScript>();
  }
}

void LuaScriptingSystem::update(f32 dt, SystemView &view) {
  QUOLL_PROFILE_EVENT("LuaScriptingSystem::update");

  mScriptLoop.onUpdate().notify(dt);
}

void LuaScriptingSystem::cleanup(SystemView &view) {
  auto &entityDatabase = view.scene->entityDatabase;

  entityDatabase.defer_begin();
  view.luaScripting.queryScripts.each(
      [this](flecs::entity entity, LuaScript &script) {
        destroyScriptingData(script);
        entity.remove<LuaScript>();
      });
  entityDatabase.defer_end();
}

void LuaScriptingSystem::createSystemViewData(SystemView &view) {
  view.scene->entityDatabase.observer<LuaScript>()
      .event(flecs::OnRemove)
      .each([this](auto &script) { destroyScriptingData(script); });

  view.luaScripting.queryScripts =
      view.scene->entityDatabase.query<LuaScript>();
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
