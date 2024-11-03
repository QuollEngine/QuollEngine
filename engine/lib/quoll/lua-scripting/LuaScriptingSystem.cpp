#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "quoll/core/Profiler.h"
#include "quoll/asset/AssetCache.h"
#include "quoll/entity/EntityDatabase.h"
#include "quoll/scene/Scene.h"
#include "quoll/system/SystemView.h"
#include "LuaScriptingSystem.h"
#include "ScriptDecorator.h"

namespace quoll {

LuaScriptingSystem::LuaScriptingSystem(AssetCache &assetCache)
    : mAssetCache(assetCache) {}

void LuaScriptingSystem::start(SystemView &view, PhysicsSystem &physicsSystem,
                               WindowSignals &windowSignals) {
  QUOLL_PROFILE_EVENT("LuaScriptingSystem::start");
  auto &entityDatabase = view.scene->entityDatabase;

  ScriptGlobals scriptGlobals{windowSignals, entityDatabase, physicsSystem,
                              mAssetCache, mScriptLoop};

  lua::ScriptDecorator scriptDecorator;
  std::vector<lua::DeferredLoader *> loaders;

  for (auto [entity, ref] : entityDatabase.view<LuaScriptAssetRef>()) {
    if (entityDatabase.has<LuaScriptCurrentAsset>(entity) &&
        entityDatabase.get<LuaScriptCurrentAsset>(entity).handle ==
            ref.asset.handle()) {
      continue;
    }

    bool valid = true;
    auto &asset = ref.asset.get();
    for (auto &[key, value] : asset.variables) {
      auto it = ref.variables.find(key);
      if (it == ref.variables.end() || !it->second.isType(value.type)) {
        valid = false;
        break;
      }
    }

    if (!valid) {
      continue;
    }

    LuaScript script{
        .variables = ref.variables,
    };

    script.loader = [this, scriptGlobals, &scriptDecorator, &ref, entity]() {
      bool loaded = false;
      while (!loaded) {
        loaded = true;
        if (ref.asset) {
          continue;
        }

        for (const auto &[key, value] : ref.variables) {
          if (value.isType(LuaScriptVariableType::AssetTexture) &&
              !value.get<AssetRef<TextureAsset>>()) {
            loaded = false;
            break;
          }

          if (value.isType(LuaScriptVariableType::AssetPrefab) &&
              !value.get<AssetRef<PrefabAsset>>()) {
            loaded = false;
            break;
          }
        }
      }

      auto &component = scriptGlobals.entityDatabase.get<LuaScript>(entity);

      if (component.state) {
        mLuaInterpreter.destroyState(component.state);
      }
      component.state = mLuaInterpreter.createState();
      auto state = sol::state_view(component.state);

      scriptDecorator.attachToScope(state, entity, scriptGlobals);
      scriptDecorator.attachVariableInjectors(state, component.variables);

      mLuaInterpreter.evaluate(ref.asset->bytes, component.state);
      scriptDecorator.removeVariableInjectors(state);
    };

    if (entityDatabase.has<LuaScript>(entity)) {
      destroyScriptingData(entityDatabase.get<LuaScript>(entity));
    }

    entityDatabase.set(entity, script);
    entityDatabase.set(entity, LuaScriptCurrentAsset{ref.asset.handle()});

    loaders.push_back(&entityDatabase.get<LuaScript>(entity).loader);
  }

  for (auto *loader : loaders) {
    loader->wait();
  }
}

void LuaScriptingSystem::update(f32 dt, SystemView &view) {
  QUOLL_PROFILE_EVENT("LuaScriptingSystem::update");

  for (auto [entity, script] : view.luaScripting.scriptRemoveObserver) {
    if (view.scene->entityDatabase.has<LuaScript>(entity)) {
      destroyScriptingData(view.scene->entityDatabase.get<LuaScript>(entity));
      view.scene->entityDatabase.remove<LuaScript>(entity);
    }

    if (view.scene->entityDatabase.has<LuaScriptCurrentAsset>(entity)) {
      view.scene->entityDatabase.remove<LuaScriptCurrentAsset>(entity);
    }
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
  entityDatabase.destroyComponents<LuaScriptCurrentAsset>();
}

void LuaScriptingSystem::createSystemViewData(SystemView &view) {
  view.luaScripting.scriptRemoveObserver =
      view.scene->entityDatabase.observeRemove<LuaScriptAssetRef>();
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
