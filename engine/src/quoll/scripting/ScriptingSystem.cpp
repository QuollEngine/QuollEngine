#include "quoll/core/Base.h"
#include "ScriptingSystem.h"
#include "ScriptDecorator.h"

#include "quoll/core/Engine.h"

namespace quoll {

ScriptingSystem::ScriptingSystem(EventSystem &eventSystem,
                                 AssetRegistry &assetRegistry)
    : mEventSystem(eventSystem), mAssetRegistry(assetRegistry) {}

void ScriptingSystem::start(EntityDatabase &entityDatabase,
                            PhysicsSystem &physicsSystem) {
  ScriptGlobals scriptGlobals{entityDatabase, physicsSystem, mAssetRegistry};
  QUOLL_PROFILE_EVENT("ScriptingSystem::start");
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

    if (component.state) {
      mLuaInterpreter.destroyState(component.state);
    }
    component.state = mLuaInterpreter.createState();
    auto state = sol::state_view(component.state);

    scriptDecorator.attachToScope(state, entity, scriptGlobals);
    scriptDecorator.attachVariableInjectors(state, component.variables);

    bool success = mLuaInterpreter.evaluate(script.data.bytes, component.state);
    QuollAssert(success, "Cannot evaluate script");

    scriptDecorator.removeVariableInjectors(state);

    createScriptingData(component, entity);
    auto res = state["start"]();
    if (!res.valid()) {
      sol::error error = res;
      Engine::getUserLogger().error() << error.what();
    }
  }

  for (auto entity : deleteList) {
    entityDatabase.remove<Script>(entity);
  }
}

void ScriptingSystem::update(f32 dt, EntityDatabase &entityDatabase) {
  QUOLL_PROFILE_EVENT("ScriptingSystem::update");

  for (auto [entity, script] : mScriptRemoveObserver) {
    destroyScriptingData(script);
  }
  mScriptRemoveObserver.clear();

  for (auto [entity, component] : entityDatabase.view<Script>()) {
    auto state = sol::state_view(component.state);
    auto res = state["update"](dt);
    if (!res.valid()) {
      sol::error error = res;
      Engine::getUserLogger().error() << error.what();
    }
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
  auto state = sol::state_view(component.state);

  if (state["on_collision_start"].get_type() == sol::type::function) {
    component.onCollisionStart = mEventSystem.observe(
        CollisionEvent::CollisionStarted,
        [this, &component, entity](const CollisionObject &data) {
          if (data.a == entity || data.b == entity) {
            auto state = sol::state_view(component.state);
            Entity target = data.a == entity ? data.b : data.a;
            auto table = state.create_table_with("target", target);
            state["on_collision_start"](table);
          }
        });
  }

  if (state["on_collision_end"].get_type() == sol::type::function) {
    component.onCollisionEnd = mEventSystem.observe(
        CollisionEvent::CollisionEnded,
        [this, &component, entity](const CollisionObject &data) {
          auto state = sol::state_view(component.state);

          if (data.a == entity || data.b == entity) {
            Entity target = data.a == entity ? data.b : data.a;
            auto table = state.create_table_with("target", target);
            state["on_collision_end"](table);
          }
        });
  }

  if (state["on_key_press"].get_type() == sol::type::function) {
    component.onKeyPress = mEventSystem.observe(
        KeyboardEvent::Pressed, [this, &component](const auto &data) {
          auto state = sol::state_view(component.state);

          auto table =
              state.create_table_with("key", data.key, "mods", data.mods);
          state["on_key_press"](table);
        });
  }

  if (state["on_key_release"].get_type() == sol::type::function) {
    component.onKeyRelease = mEventSystem.observe(
        KeyboardEvent::Released, [this, &component](const auto &data) {
          auto state = sol::state_view(component.state);

          auto table =
              state.create_table_with("key", data.key, "mods", data.mods);
          state["on_key_release"](table);
        });
  }
}

void ScriptingSystem::destroyScriptingData(Script &component) {
  if (component.state) {
    mLuaInterpreter.destroyState(component.state);
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
