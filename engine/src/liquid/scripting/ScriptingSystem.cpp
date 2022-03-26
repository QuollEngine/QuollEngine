#include "liquid/core/Base.h"
#include "ScriptingSystem.h"
#include "ScriptingUtils.h"

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

        mLuaInterpreter.callFunction(component.scope, "start");
      });
}

void ScriptingSystem::update() {
  mEntityContext.iterateEntities<ScriptingComponent>(
      [this](auto entity, const ScriptingComponent &component) {
        auto &script = mScripts.at(component.handle);

        mLuaInterpreter.callFunction(component.scope, "update");
      });
}

void ScriptingSystem::createScriptingData(ScriptingComponent &component,
                                          Entity entity) {
  if (mLuaInterpreter.hasFunction(component.scope, "on_collision_start")) {
    component.onCollisionStart = mEventSystem.observe(
        CollisionEvent::CollisionStarted,
        [this, &component, entity](const CollisionObject &data) {
          if (data.a == entity || data.b == entity) {
            mLuaInterpreter.callFunction(component.scope, "on_collision_start");
          }
        });
  }

  if (mLuaInterpreter.hasFunction(component.scope, "on_collision_end")) {
    component.onCollisionEnd = mEventSystem.observe(
        CollisionEvent::CollisionEnded,
        [this, &component, entity](const CollisionObject &data) {
          if (data.a == entity || data.b == entity) {
            mLuaInterpreter.callFunction(component.scope, "on_collision_end");
          }
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
}

} // namespace liquid
