#include "quoll/core/Base.h"
#include "quoll/entity/EntityLuaTable.h"

#include "PhysicsSystemLuaTable.h"
#include "CollisionEvent.h"

namespace quoll {

PhysicsSystemLuaTable::PhysicsSystemLuaTable(Entity entity,
                                             ScriptGlobals scriptGlobals)
    : mEntity(entity), mScriptGlobals(scriptGlobals) {}

PhysicsSystemLuaTable
PhysicsSystemLuaTable::create(sol::state_view state, Entity entity,
                              ScriptGlobals scriptGlobals) {
  auto collisionEvent = state.new_usertype<CollisionEvent>(sol::no_constructor);
  collisionEvent["a"] = sol::property([scriptGlobals](CollisionEvent &event) {
    return EntityLuaTable(event.a, scriptGlobals);
  });

  collisionEvent["b"] = sol::property([scriptGlobals](CollisionEvent &event) {
    return EntityLuaTable(event.b, scriptGlobals);
  });

  auto usertype =
      state.new_usertype<PhysicsSystemLuaTable>(sol::no_constructor);
  usertype["on_collision_start"] =
      sol::property(&PhysicsSystemLuaTable::onCollisionStart);
  usertype["on_collision_end"] =
      sol::property(&PhysicsSystemLuaTable::onCollisionEnd);

  return PhysicsSystemLuaTable(entity, scriptGlobals);
}

lua::ScriptSignalView PhysicsSystemLuaTable::onCollisionStart() {
  auto &script = mScriptGlobals.entityDatabase.get<LuaScript>(mEntity);
  return lua::ScriptSignalView(
      mScriptGlobals.physicsSystem.getSignals().getCollisionStartSignal(),
      script);
}

lua::ScriptSignalView PhysicsSystemLuaTable::onCollisionEnd() {
  auto &script = mScriptGlobals.entityDatabase.get<LuaScript>(mEntity);
  return lua::ScriptSignalView(
      mScriptGlobals.physicsSystem.getSignals().getCollisionEndSignal(),
      script);
}

} // namespace quoll
