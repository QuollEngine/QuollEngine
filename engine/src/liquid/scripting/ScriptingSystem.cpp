#include "liquid/core/Base.h"
#include "ScriptingSystem.h"
#include "ScriptingUtils.h"
#include "LuaTable.h"

#include "liquid/core/EngineGlobals.h"

namespace liquid {

ScriptingSystem::ScriptingSystem(EntityContext &entityContext,
                                 EventSystem &eventSystem)
    : mEntityContext(entityContext), mEventSystem(eventSystem) {}

ScriptingSystem::~ScriptingSystem() {
  mEntityContext.iterateEntities<ScriptingComponent>(
      [this](auto entity, ScriptingComponent &scripting) {
        destroyScriptingData(scripting);
      });
  mEntityContext.destroyComponents<ScriptingComponent>();
}

ScriptHandle ScriptingSystem::addScript(const String &fileName) {
  auto bytes = utils::readFileIntoBuffer(fileName);
  return addScript(fileName, bytes);
}

ScriptHandle ScriptingSystem::addScript(const String &name,
                                        const std::vector<char> &bytes) {
  ScriptHandle current = mLastHandle;

  mScripts.insert_or_assign(current, Script{ScriptType::Lua, name, bytes});

  mLastHandle = ScriptHandle{static_cast<uint32_t>(mLastHandle) + 1};
  return current;
}

void ScriptingSystem::start() {
  LIQUID_PROFILE_EVENT("ScriptingSystem::start");
  mEntityContext.iterateEntities<ScriptingComponent>(
      [this](auto entity, ScriptingComponent &component) {
        if (component.started) {
          return;
        }

        component.started = true;

        if (component.scope) {
          destroyScriptingData(component);
        }
        component.scope = mLuaInterpreter.createScope();

        auto &script = mScripts.at(component.handle);
        mLuaInterpreter.evaluate(script.bytes, component.scope);

        createScriptingData(component, entity);

        mLuaInterpreter.getFunction(component.scope, "start");
        mLuaInterpreter.callFunction(component.scope, 0);
      });
}

void ScriptingSystem::update() {
  LIQUID_PROFILE_EVENT("ScriptingSystem::update");
  mEntityContext.iterateEntities<ScriptingComponent>(
      [this](auto entity, const ScriptingComponent &component) {
        auto &script = mScripts.at(component.handle);

        mLuaInterpreter.getFunction(component.scope, "update");
        mLuaInterpreter.callFunction(component.scope, 0);
      });
}

void ScriptingSystem::createScriptingData(ScriptingComponent &component,
                                          Entity entity) {
  if (mLuaInterpreter.hasFunction(component.scope, "on_collision_start")) {
    component.onCollisionStart = mEventSystem.observe(
        CollisionEvent::CollisionStarted,
        [this, &component, entity](const CollisionObject &data) {
          if (data.a == entity || data.b == entity) {
            Entity target = data.a == entity ? data.b : data.a;

            mLuaInterpreter.getFunction(component.scope, "on_collision_start");
            LuaTable table(component.scope, 1);
            table.set("target", target);
            mLuaInterpreter.callFunction(component.scope, 1);
          }
        });
  }

  if (mLuaInterpreter.hasFunction(component.scope, "on_collision_end")) {
    component.onCollisionEnd = mEventSystem.observe(
        CollisionEvent::CollisionEnded,
        [this, &component, entity](const CollisionObject &data) {
          if (data.a == entity || data.b == entity) {
            Entity target = data.a == entity ? data.b : data.a;
            mLuaInterpreter.getFunction(component.scope, "on_collision_end");
            LuaTable table(component.scope, 1);
            table.set("target", target);
            mLuaInterpreter.callFunction(component.scope, 1);
          }
        });
  }

  if (mLuaInterpreter.hasFunction(component.scope, "on_key_press")) {
    component.onKeyPress = mEventSystem.observe(
        KeyboardEvent::Pressed, [this, &component](const auto &data) {
          mLuaInterpreter.getFunction(component.scope, "on_key_press");
          LuaTable table(component.scope, 1);
          table.set("key", data.key);
          mLuaInterpreter.callFunction(component.scope, 1);
        });
  }

  if (mLuaInterpreter.hasFunction(component.scope, "on_key_release")) {
    component.onKeyRelease = mEventSystem.observe(
        KeyboardEvent::Released, [this, &component](const auto &data) {
          mLuaInterpreter.getFunction(component.scope, "on_key_release");
          LuaTable table(component.scope, 1);
          table.set("key", data.key);
          mLuaInterpreter.callFunction(component.scope, 1);
        });
  }
}

void ScriptingSystem::destroyScriptingData(ScriptingComponent &component) {
  mLuaInterpreter.destroyScope(component.scope);
  if (component.onCollisionStart != EVENT_OBSERVER_MAX) {
    mEventSystem.removeObserver(CollisionEvent::CollisionStarted,
                                component.onCollisionStart);
  }

  if (component.onCollisionEnd != EVENT_OBSERVER_MAX) {
    mEventSystem.removeObserver(CollisionEvent::CollisionEnded,
                                component.onCollisionEnd);
  }

  if (component.onKeyPress != EVENT_OBSERVER_MAX) {
    mEventSystem.removeObserver(KeyboardEvent::Pressed, component.onKeyPress);
  }

  if (component.onKeyRelease != EVENT_OBSERVER_MAX) {
    mEventSystem.removeObserver(KeyboardEvent::Released,
                                component.onKeyRelease);
  }
}

} // namespace liquid
