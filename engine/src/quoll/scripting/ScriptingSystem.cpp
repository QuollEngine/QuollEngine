#include "quoll/core/Base.h"
#include "ScriptingSystem.h"
#include "LuaTable.h"
#include "ScriptDecorator.h"

#include "quoll/core/Engine.h"

namespace quoll {

ScriptingSystem::ScriptingSystem(EventSystem &eventSystem,
                                 AssetRegistry &assetRegistry)
    : mEventSystem(eventSystem), mAssetRegistry(assetRegistry) {}

void ScriptingSystem::start(EntityDatabase &entityDatabase) {
  LIQUID_PROFILE_EVENT("ScriptingSystem::start");
  ScriptDecorator scriptDecorator;
  std::vector<Entity> deleteList;
  for (auto [entity, component] : entityDatabase.view<Script>()) {
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
      continue;
    }

    component.started = true;

    if (component.scope.getLuaState()) {
      mLuaInterpreter.destroyScope(component.scope);
    }
    component.scope = mLuaInterpreter.createScope();
    scriptDecorator.attachToScope(component.scope, entity, entityDatabase,
                                  mAssetRegistry);
    scriptDecorator.attachVariableInjectors(component.scope,
                                            component.variables);

    bool success = mLuaInterpreter.evaluate(script.data.bytes, component.scope);
    LIQUID_ASSERT(success, "Cannot evaluate script");

    scriptDecorator.removeVariableInjectors(component.scope);

    createScriptingData(component, entity);
    component.scope.luaGetGlobal("start");
    component.scope.call(0);
  }

  for (auto entity : deleteList) {
    entityDatabase.remove<Script>(entity);
  }
}

void ScriptingSystem::update(float dt, EntityDatabase &entityDatabase) {
  LIQUID_PROFILE_EVENT("ScriptingSystem::update");

  for (auto [entity, script] : mScriptRemoveObserver) {
    destroyScriptingData(script);
  }
  mScriptRemoveObserver.clear();

  for (auto [entity, component] : entityDatabase.view<Script>()) {
    component.scope.luaGetGlobal("update");
    component.scope.set(dt);
    component.scope.call(1);
  }
}

void ScriptingSystem::cleanup(EntityDatabase &entityDatabase) {
  for (auto [entity, script] : entityDatabase.view<Script>()) {
    destroyScriptingData(script);
  }

  entityDatabase.destroyComponents<Script>();
}

void ScriptingSystem::observeChanges(EntityDatabase &entityDatabase) {
  mScriptRemoveObserver = entityDatabase.observeRemove<Script>();
}

void ScriptingSystem::createScriptingData(Script &component, Entity entity) {
  if (component.scope.hasFunction("on_collision_start")) {
    component.onCollisionStart = mEventSystem.observe(
        CollisionEvent::CollisionStarted,
        [this, &component, entity](const CollisionObject &data) {
          if (data.a == entity || data.b == entity) {
            component.scope.luaGetGlobal("on_collision_start");
            Entity target = data.a == entity ? data.b : data.a;
            auto table = component.scope.createTable(1);
            table.set("target", target);

            component.scope.call(1);
          }
        });
  }

  if (component.scope.hasFunction("on_collision_end")) {
    component.onCollisionEnd = mEventSystem.observe(
        CollisionEvent::CollisionEnded,
        [this, &component, entity](const CollisionObject &data) {
          if (data.a == entity || data.b == entity) {
            component.scope.luaGetGlobal("on_collision_end");
            Entity target = data.a == entity ? data.b : data.a;
            auto table = component.scope.createTable(1);
            table.set("target", target);

            component.scope.call(1);
          }
        });
  }

  if (component.scope.hasFunction("on_key_press")) {
    component.onKeyPress = mEventSystem.observe(
        KeyboardEvent::Pressed, [this, &component](const auto &data) {
          component.scope.luaGetGlobal("on_key_press");

          auto table = component.scope.createTable(2);
          table.set("key", data.key);
          table.set("mods", data.mods);

          component.scope.call(1);
        });
  }

  if (component.scope.hasFunction("on_key_release")) {
    component.onKeyRelease = mEventSystem.observe(
        KeyboardEvent::Released, [this, &component](const auto &data) {
          component.scope.luaGetGlobal("on_key_release");
          auto table = component.scope.createTable(2);
          table.set("key", data.key);
          table.set("mods", data.mods);

          component.scope.call(1);
        });
  }
}

void ScriptingSystem::destroyScriptingData(Script &component) {
  if (component.scope) {
    mLuaInterpreter.destroyScope(component.scope);
  }

  if (component.onCollisionStart != EventObserverMax) {
    mEventSystem.removeObserver(CollisionEvent::CollisionStarted,
                                component.onCollisionStart);
  }

  if (component.onCollisionEnd != EventObserverMax) {
    mEventSystem.removeObserver(CollisionEvent::CollisionEnded,
                                component.onCollisionEnd);
  }

  if (component.onKeyPress != EventObserverMax) {
    mEventSystem.removeObserver(KeyboardEvent::Pressed, component.onKeyPress);
  }

  if (component.onKeyRelease != EventObserverMax) {
    mEventSystem.removeObserver(KeyboardEvent::Released,
                                component.onKeyRelease);
  }
}

} // namespace quoll
