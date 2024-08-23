#include "quoll/core/Base.h"
#include "quoll/entity/EntityLuaTable.h"
#include "quoll/physics/PhysicsSystem.h"
#include "CollisionEvent.h"
#include "PhysicsSystemLuaTable.h"

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
  usertype["onCollisionStart"] =
      sol::property(&PhysicsSystemLuaTable::onCollisionStart);
  usertype["onCollisionEnd"] =
      sol::property(&PhysicsSystemLuaTable::onCollisionEnd);

  return PhysicsSystemLuaTable(entity, scriptGlobals);
}

SignalLuaTable PhysicsSystemLuaTable::onCollisionStart() {
  return SignalLuaTable(
      mScriptGlobals.physicsSystem.getSignals().onCollisionStart(), mEntity);
}

SignalLuaTable PhysicsSystemLuaTable::onCollisionEnd() {
  return SignalLuaTable(
      mScriptGlobals.physicsSystem.getSignals().onCollisionEnd(), mEntity);
}

} // namespace quoll
